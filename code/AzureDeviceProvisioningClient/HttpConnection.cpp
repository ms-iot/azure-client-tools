// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "stdafx.h"
#include "HttpConnection.h"

using namespace Microsoft::WRL;
using namespace std;
using namespace std::chrono;
using namespace web;
using namespace web::http;
using namespace web::http::details;
using namespace web::http::client;
using namespace web::json;
using namespace wil;

namespace Microsoft { namespace Windows { namespace IoT { namespace DeviceManagement { namespace Provisioning {

    static const utf16string USER_AGENT_VALUE{ L"iothub_dps_client/1.0" };
    static const utf16string KEEP_ALIVE_VALUE{L"keep-alive"};


    void CAzureDpsHTTPConnection::init_config() {
        web_proxy proxy{};  // defaults is WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY which does the right thing on 8.1+ with system and user settings, wpad, etc. per msdn
        m_config.set_proxy(proxy);
        // this whole tpm sas challenge song & dance authenticates the client and the
        // https channel is solely encrypted by the remote server cert

        m_config.set_nativehandle_options([](native_handle handle)->void {
            BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::NativeHandleOptions)
                if (!WinHttpSetOption(handle, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, WINHTTP_NO_CLIENT_CERT_CONTEXT, 0)) {
                    THROW_LAST_ERROR();
                }
            // TODO: should we require a minimum of tls 1.2 or just use the default??
            ACTIVITY_CATCH_ALL_CONVERTING_TO_HR_AND_FAIL_FAST

        });
    }

    void CAzureDpsHTTPConnection::add_handler(http_client& c) {
        auto apply_headers{
            [&](http_request request, std::shared_ptr<http_pipeline_stage> next_stage)->pplx::task<http_response> {
            BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::HttpHeaderInsertion)
                request.headers().add(header_names::user_agent, USER_AGENT_VALUE);
                request.headers().add(header_names::accept, mime_types::application_json);
                request.headers().add(header_names::connection, KEEP_ALIVE_VALUE);
                // request creation overload sets correct content type

                if (!m_sas_token.empty()) {
                    request.headers().add(header_names::authorization, m_sas_token);
                    CTelemetryProvider::ChallengeSASTokenHeader(m_sas_token.c_str());
                }
            ACTIVITY_CATCH_ALL_CONVERTING_TO_HR_AND_FAIL_FAST

            return next_stage->propagate(request);
        } };
        c.add_handler(apply_headers);
    }

    CAzureDpsRegisterHTTPConnection::CAzureDpsRegisterHTTPConnection(CAzureDpsRegisterInstance& info) : m_dps(info) {
        m_base_dps_uri.set_scheme(L"https");

        m_base_dps_uri.set_host(m_dps.get_dps_uri());
        m_base_dps_uri.set_port(DPS_HTTP_PORT_NUM);

        init_registration_body();
        init_config();
    }

    uri CAzureDpsRegisterHTTPConnection::get_current_request_uri(const wstring& scope_id, const wstring& registration_id, const wstring& operationId,
                                                 const value& registration_body, method& method, value& body) const {
        uri_builder azure_dps_uri{m_base_dps_uri};
        azure_dps_uri.append_path(scope_id, true);
        azure_dps_uri.append_path(L"registrations");
        azure_dps_uri.append_path(registration_id, true);
        if (operationId.empty()) {
            //static const char* DPS_REGISTRATION_URI_FMT = "/%s/registrations/%s/register?api-version=%s";
            azure_dps_uri.append_path(L"register");
            body = registration_body;
            method = methods::PUT;
        } else {
            //static const char* DPS_OP_STATUS_URI_FMT = "/%s/registrations/%s/operations/%s?api-version=%s";
            azure_dps_uri.append_path(L"operations");
            azure_dps_uri.append_path(operationId, true);
            method = methods::GET;
        }
        azure_dps_uri.append_query(L"api-version", DPS_API_VERSION, true);
        return azure_dps_uri.to_uri();
    }

    void CAzureDpsRegisterHTTPConnection::init_registration_body() {
        auto tpm_attestation = value::object();
        tpm_attestation[L"endorsementKey"] = value::string(utility::conversions::to_base64(m_dps.get_ek()));
        tpm_attestation[L"storageRootKey"] = value::string(utility::conversions::to_base64(m_dps.get_srk()));
        m_registration_body = value::object();
        m_registration_body[L"registrationId"] = value::string(m_dps.get_registration_id());
        m_registration_body[L"tpm"] = tpm_attestation;
    }

    static const char* JSON_NODE_STATUS = "status";
    static const char* JSON_NODE_REG_STATUS = "registrationStatus";
    static const char* JSON_NODE_AUTH_KEY = "authenticationKey";
    static const char* JSON_NODE_DEVICE_ID = "deviceId";
    static const char* JSON_NODE_KEY_NAME = "keyName";
    static const char* JSON_NODE_OPERATION_ID = "operationId";
    static const char* JSON_NODE_ASSIGNED_HUB = "assignedHub";
    static const char* JSON_NODE_TPM_NODE = "tpm";
    static const char* JSON_NODE_TRACKING_ID = "trackingId";

    static const char* DPS_ASSIGNED_STATUS = "assigned";
    static const char* DPS_ASSIGNING_STATUS = "assigning";
    static const char* DPS_UNASSIGNED_STATUS = "unassigned";
    static const char* DPS_FAILED_STATUS = "failed";
    // docs say this should be "disabled", sdk sample shows "blacklisted" why the difference?
    static const char* DPS_BLACKLISTED_STATUS = "blacklisted";

    http_client CAzureDpsRegisterHTTPConnection::make_client_request(method& request_method, json::value& request_body, registration_result_t& final_result) {


        uri cur_request_uri{get_current_request_uri(m_dps.get_scope_id(), m_dps.get_registration_id(), final_result.operationId,
                                                    m_registration_body, request_method, request_body)};

        CTelemetryProvider::AzureClientRequestURI(request_method == methods::PUT ? L"PUT " : L"GET ", cur_request_uri.to_string().c_str());

        http_client azure_dps(cur_request_uri, m_config);
        add_handler(azure_dps);

        auto body_text = utility::conversions::to_utf8string(request_body.serialize());
        auto length = body_text.size();
        CTelemetryProvider::AzureClientRequestBody(length, request_body.serialize().c_str());
        return azure_dps;
    }

    void CAzureDpsRegisterHTTPConnection::process_response(const http_response& response, web::http::status_code& http_status_code) const {
        http_status_code = response.status_code();
        CTelemetryProvider::AzureClientResponseCode(m_http_status_code);
        switch (m_http_status_code) {
        case status_codes::Accepted:
        case status_codes::OK:
        case status_codes::Unauthorized:
        break;
        default:
            throw http_exception(response.status_code());
        }
    }

    CAzureDpsRegisterHTTPConnection::actual_result CAzureDpsRegisterHTTPConnection::process_unassigned(web::json::object& result, std::wstring& sas_token) const {
        registration_result_t fr{};
        auto i{result.find(L"authenticationKey")};
        if (i == result.end()) {
            sas_token.clear();
            auto i2{result.find(L"errorCode")};
            if (i2 != result.end()) {
                fr.errorCode = (HRESULT)(i2->second).as_integer();
            } else {
                fr.errorCode = MISSING_ERROR_CODE_UNAUTHORIZED_MISSING_KEY;
            }
            i2 = result.find(L"message");
            if (i2 != result.end()) {
                fr.errorMessage = (i2->second).as_string();
            } else {
                fr.errorMessage.clear();
            }
            CTelemetryProvider::UnassignedWithoutChallengeKey(fr.errorCode, fr.errorMessage.c_str());

            return actual_result(fr, 1); // retry
        }
        const wstring base64_encoded_key{(i->second).as_string()};
        CTelemetryProvider::UnassignedWithChallengeKey(base64_encoded_key.c_str());

        auto keyName{utility::conversions::utf16_to_utf8(result[L"keyName"].as_string())};

        sas_token = m_dps.GenerateSASToken(base64_encoded_key);
        sas_token.resize(wcslen(sas_token.c_str()));

        CTelemetryProvider::SASTokenPrepared(sas_token.c_str());
        return actual_result(fr, 0);
    }

    CAzureDpsRegisterHTTPConnection::actual_result CAzureDpsRegisterHTTPConnection::process_assigned(web::json::object& result) const {
        registration_result_t fr{};
        auto registration_status{result[L"registrationState"].as_object()};
        auto tpm{registration_status[L"tpm"].as_object()};
        fr.authenticationKey = tpm[L"authenticationKey"].as_string();
        fr.assignedHub = registration_status[L"assignedHub"].as_string();
        fr.deviceId = registration_status[L"deviceId"].as_string();
        fr.deviceRegistrationStatus = registration_status[L"status"].as_string();
        CTelemetryProvider::Assigned(fr.authenticationKey.c_str(), fr.assignedHub.c_str(), fr.deviceId.c_str());
        return actual_result(fr, 0);
    }

    CAzureDpsRegisterHTTPConnection::actual_result CAzureDpsRegisterHTTPConnection::process_assigning(web::json::object& result) const {
        registration_result_t fr{};

        fr.operationId = result[L"operationId"].as_string();
        CTelemetryProvider::AssignmentInProgress(fr.operationId.c_str());
        ::Sleep(UNASSIGNED_OP_WAIT);
        return actual_result(fr, 0); // retry
    }

    CAzureDpsRegisterHTTPConnection::actual_result CAzureDpsRegisterHTTPConnection::process_response_body(const value& response_body) {
        registration_result_t fr{};
        BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::ProcessHttpStatusBody)
            CTelemetryProvider::AzureClientResponseBody(response_body.serialize().c_str());
            OutputDebugString(L"responseBody");
            OutputDebugString(response_body.serialize().c_str());
            if (response_body.is_null()) {
                return actual_result(fr, 1);
            }
            // possible results, assigned, unassigned, assigning, failed, disable/blacklist??
            auto result{response_body.as_object()};
            wstring dps_status;
            auto i{result.find(L"status")};
            if (i != result.end()) {
                dps_status = (i->second).as_string();
            };
            if (dps_status.empty() || dps_status == L"unassigned") {
                return process_unassigned(result, m_sas_token);
            } else if (dps_status == L"assigned") {
                return process_assigned(result);
            } else if (dps_status == L"assigning") {
                return process_assigning(result);
            } else if (dps_status == L"failed") {
                auto i2{result.find(L"errorCode")};
                if (i2 != result.end()) {
                    fr.errorCode = (HRESULT)(i2->second).as_integer();
                } else {
                    fr.errorCode = MISSING_ERROR_CODE_EXPLICIT_FAILURE_RESPONSE;
                }
                i2 = result.find(L"message");
                if (i2 != result.end()) {
                    fr.errorMessage = (i2->second).as_string();
                } else {
                    fr.errorMessage.clear();
                }
                CTelemetryProvider::Failed(fr.errorCode, fr.errorMessage.c_str());
                return actual_result(fr, 1); // retry
            } else if (dps_status == L"blacklisted") {  // TODO: figure out why docs and sdk sample disagree about the name of this value
                fr.errorCode = MISSING_ERROR_CODE_EXPLICIT_BLACKLIST;
                fr.errorMessage = dps_status;
                return actual_result(fr, 1); //retry
            } else {
                //__debugbreak();
                // unknown response
                throw std::runtime_error("unknown dps_status response");
            }
        ACTIVITY_CATCH_ALL_CONVERTING_TO_HR
        return actual_result(fr, 1); //retry
    }

    registration_result_t CAzureDpsRegisterHTTPConnection::execute_register_request() {
        auto start_time = steady_clock::now();

        registration_result_t final_result{0};

        int failure_count{};

        method request_method{methods::PUT};
        while (final_result.assignedHub.empty() &&
               duration_cast<seconds>(steady_clock::now() - start_time) < DEFAULT_REGISTRATION_TIMEOUT &&
               failure_count < MAX_FAILURE_COUNT) {
            json::value request_body{};
            auto azure_dps = make_client_request(request_method, request_body, final_result);

            auto result = azure_dps.request(request_method, wstring(), request_body).then([&](http_response response)->pplx::task<json::value> {
                BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::ProcessHttpStatusResponse)
                    process_response(response, m_http_status_code);
                ACTIVITY_CATCH_ALL_CONVERTING_TO_HR_AND_FAIL_FAST
                return response.extract_json();
            }).then([&](json::value r) {
                return process_response_body(r);
            }).get();
            failure_count += result.second;
            final_result = result.first;
        } // retry until success or timeout
        return final_result;
    }

    CAzureDpsEnrollmentHTTPConnection::CAzureDpsEnrollmentHTTPConnection(CAzureDpsEnrollmentInstance& info) : m_dps(info) {
        m_base_dps_uri.set_scheme(L"https");

        m_base_dps_uri.set_host(m_dps.get_dps_uri());
        m_base_dps_uri.set_port(DPS_HTTP_PORT_NUM);
        m_sas_token = info.GenerateSASToken();
        init_config();
    }

    uri CAzureDpsEnrollmentHTTPConnection::get_current_request_uri(
        const wstring& registration_id,
        const value& registration_body,
        method& method,
        value& body) const
    {
        uri_builder azure_dps_uri{ m_base_dps_uri };
        azure_dps_uri.append_path(L"enrollments");
        azure_dps_uri.append_path(registration_id, true);
        body = registration_body;
        azure_dps_uri.append_query(L"api-version", DPS_API_VERSION, true);
        OutputDebugString(L"-----request uri get_current_request_uri----\n");
        OutputDebugString(azure_dps_uri.to_uri().to_string().c_str());
        return azure_dps_uri.to_uri();
    }

    void CAzureDpsEnrollmentHTTPConnection::init_registration_body() {
        m_registration_body = value::object();

        m_registration_body[L"registrationId"] = value::string(m_dps.get_registration_id());

        if (!m_dps.get_device_id().empty())
        {
            m_registration_body[L"deviceId"] = value::string(m_dps.get_device_id());
        }

        m_registration_body[L"provisioningStatus"] = value::string(L"enabled");

        auto attestation = value::object();
        attestation[L"type"] = value::string(L"tpm");

        auto tpm_attestation = value::object();
        tpm_attestation[L"endorsementKey"] = value::string(utility::conversions::to_base64(m_dps.get_ek()));
        tpm_attestation[L"storageRootKey"] = value::string(utility::conversions::to_base64(m_dps.get_srk()));

        attestation[L"tpm"] = tpm_attestation;

        m_registration_body[L"attestation"] = attestation;

        OutputDebugString(L"-----m_registration_body-----\n");
        OutputDebugString(m_registration_body.serialize().c_str());
    }

    http_client CAzureDpsEnrollmentHTTPConnection::make_client_request(method& request_method, json::value& request_body, enrollment_result_t& final_result) {

        uri cur_request_uri{ get_current_request_uri(
            m_dps.get_registration_id(),
            m_registration_body,
            request_method,
            request_body) };

        CTelemetryProvider::AzureClientRequestURI(request_method.c_str(), cur_request_uri.to_string().c_str());

        http_client azure_dps(cur_request_uri, m_config);
        add_handler(azure_dps);

        auto body_text = utility::conversions::to_utf8string(request_body.serialize());
        auto length = body_text.size();
        CTelemetryProvider::AzureClientRequestBody(length, request_body.serialize().c_str());

        OutputDebugString(L"-----request body-----\n");
        OutputDebugStringA(body_text.c_str());

        OutputDebugString(L"-----request method-----\n");
        OutputDebugString(request_method.c_str());

        return azure_dps;
    }

    void CAzureDpsEnrollmentHTTPConnection::process_response(const web::http::http_response& response, const web::http::method& requestMethod, web::http::status_code& http_status_code) const {
        http_status_code = response.status_code();
        CTelemetryProvider::AzureClientResponseCode(m_http_status_code);

        if (http_status_code == status_codes::BadRequest)
        {
            throw http_exception(response.status_code());
        }
    }

    CAzureDpsEnrollmentHTTPConnection::actual_result CAzureDpsEnrollmentHTTPConnection::process_response_body(const web::json::value& response_body, const web::http::method& requestMethod)
    {
        enrollment_result_t fr{};
        int fail = 1;

        BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::ProcessHttpStatusBody)
            CTelemetryProvider::AzureClientResponseBody(response_body.serialize().c_str());
            if (response_body .is_null()) {
                if (requestMethod == methods::DEL)
                {
                    fr.deviceRegistrationStatus = L"deleted";
                    return actual_result(fr, 0);
                }
                else
                    return actual_result(fr, fail);
            }

            auto result{ response_body.as_object() };

            auto errorCode = result.find(L"errorCode");
            if (errorCode != result.end())
            {
                fr.errorCode = (HRESULT)(errorCode->second).as_integer();
                auto message = result.find(L"message");
                if (message != result.end())
                {
                    fr.errorMessage = (message->second).as_string();
                }
                auto trackingId = result.find(L"trackingId");
                if (trackingId != result.end())
                {
                    fr.errorTrackingId = (trackingId->second).as_string();
                }
            }
            else
            {
                auto registrationId = result.find(L"registrationId");
                if (registrationId != result.end())
                {
                    fr.registrationId = (registrationId->second).as_string();
                }

                auto deviceId = result.find(L"deviceId");
                if (deviceId != result.end())
                {
                    fr.deviceId = (deviceId->second).as_string();
                }
                auto provisioningStatus = result.find(L"provisioningStatus");
                if (provisioningStatus != result.end())
                {
                    fr.status = (provisioningStatus->second).as_string();
                }

                auto registrationState = result.find(L"registrationState");
                if (registrationState != result.end())
                {
                    auto state{ (registrationState->second).as_object() };
                    auto deviceRegistrationStatus = state.find(L"status");
                    if (deviceRegistrationStatus != state.end())
                    {
                        fr.deviceRegistrationStatus = (deviceRegistrationStatus->second).as_string();
                    }
                }
                fail = 0;
            }
        ACTIVITY_CATCH_ALL_CONVERTING_TO_HR
        return CAzureDpsEnrollmentHTTPConnection::actual_result(fr, fail);
    }

    enrollment_result_t CAzureDpsEnrollmentHTTPConnection::execute_enrollment_request() {
        auto start_time = steady_clock::now();
        enrollment_result_t final_result{ 0 };
        int failure_count{};

        method request_method{ methods::PUT };
        init_registration_body();
        int fail = 1;
        while (fail == 1 &&
            duration_cast<seconds>(steady_clock::now() - start_time) < DEFAULT_REGISTRATION_TIMEOUT &&
            failure_count < MAX_FAILURE_COUNT) {
            json::value request_body{};
            auto azure_dps = make_client_request(request_method, request_body, final_result);
            auto result = azure_dps.request(request_method, wstring(), request_body).then([&](http_response response)->pplx::task<json::value> {
                BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::ProcessHttpStatusResponse)
                process_response(response, request_method, m_http_status_code);
                ACTIVITY_CATCH_ALL_CONVERTING_TO_HR_AND_FAIL_FAST
                return response.extract_json();
            }).then([&](json::value r) {
                return process_response_body(r, request_method);
            }).get();
            if (result.second == 0)
            {
                fail = 0;
            }
            failure_count += result.second;
            final_result = result.first;
        } // retry until success or timeout
        return final_result;
    }

    enrollment_result_t CAzureDpsEnrollmentHTTPConnection::execute_delete_request() {
        auto start_time = steady_clock::now();
        enrollment_result_t final_result{ 0 };
        int failure_count{};

        method request_method{ methods::DEL };
        int fail = 1;
        while (fail == 1 &&
            duration_cast<seconds>(steady_clock::now() - start_time) < DEFAULT_REGISTRATION_TIMEOUT &&
            failure_count < MAX_FAILURE_COUNT) {
            json::value request_body{};
            auto azure_dps = make_client_request(request_method, request_body, final_result);
            auto result = azure_dps.request(request_method, wstring(), request_body).then([&](http_response response)->pplx::task<json::value> {
                BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::ProcessHttpStatusResponse)
                process_response(response, request_method, m_http_status_code);
                ACTIVITY_CATCH_ALL_CONVERTING_TO_HR_AND_FAIL_FAST
                return response.extract_json();
            }).then([&](json::value r) {
                return process_response_body(r, request_method);
            }).get();
            if (result.second == 0)
            {
                fail = 0;
            }
            failure_count += result.second;
            final_result = result.first;
        } // retry until success or timeout
        return final_result;
    }

    enrollment_result_t CAzureDpsEnrollmentHTTPConnection::execute_get_request() {
        auto start_time = steady_clock::now();
        enrollment_result_t final_result{ 0 };
        int failure_count{};

        method request_method{ methods::GET };
        int fail = 1;
        while (fail == 1 &&
            duration_cast<seconds>(steady_clock::now() - start_time) < DEFAULT_REGISTRATION_TIMEOUT &&
            failure_count < MAX_FAILURE_COUNT) {
            json::value request_body{};
            auto azure_dps = make_client_request(request_method, request_body, final_result);
            auto result = azure_dps.request(request_method, wstring(), request_body).then([&](http_response response)->pplx::task<json::value> {
                BEGIN_GUARDED_ACTIVITY(CTelemetryProvider::ProcessHttpStatusResponse)
                process_response(response, request_method, m_http_status_code);
                ACTIVITY_CATCH_ALL_CONVERTING_TO_HR_AND_FAIL_FAST
                return response.extract_json();
            }).then([&](json::value r) {
                return process_response_body(r, request_method);
            }).get();
            if (result.second == 0)
            {
                fail = 0;
            }
            failure_count += result.second;
            final_result = result.first;
        } // retry until success or timeout
        return final_result;
    }


}}}}};
