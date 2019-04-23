// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DMBridgeServer.h"
#include "RpcConstants.h"

#include "utilities/DMException.h"

// The capability that UWP apps must have inorder to be able to use the Rpc endpoints
constexpr wchar_t* REQUIRED_CAPABILITY = L"systemManagement";
constexpr int RPC_MIN_CALLS = 1;
constexpr int RPC_DONT_WAIT = 0;

using namespace std;
using namespace Microsoft::Azure::DeviceManagement::Utils;

shared_ptr<Microsoft::Azure::DeviceManagement::Common::ILocalDMClient> DMBridgeServer::_localDmAccess;

void DMBridgeServer::Setup(shared_ptr<Microsoft::Azure::DeviceManagement::Common::ILocalDMClient> localDmAccess)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);

    _localDmAccess = localDmAccess;

    SECURITY_DESCRIPTOR rpcSecurityDescriptor;
    RPC_STATUS status = RPC_S_OK;

    try
    {
        rpcSecurityDescriptor = GenerateSecurityDescriptor(REQUIRED_CAPABILITY);
    }
    catch (const DMException)
    {
        TRACELINE(LoggingLevel::Verbose, L"Error: Failed to generate security descriptor.");
        throw;
    }
    catch (const exception& ex)
    {
        TRACELINEP(
            LoggingLevel::Verbose,
            ex.what(),
            GetLastError());
        throw;
    }

    status = RpcServerUseProtseqEp(
        (RPC_WSTR)RPC_PROTOCOL_SEQUENCE,
        RPC_C_PROTSEQ_MAX_REQS_DEFAULT,
        (RPC_WSTR)RPC_ENDPOINT,
        &rpcSecurityDescriptor);

    if (status != RPC_S_OK)
    {
        throw DMException(DMSubsystem::RPC, status, "Failed to setup RPC protocol");
    }

    try
    {
        vector<RPC_IF_HANDLE> interfaces = {
            { Dps_v1_0_s_ifspec },
            { Reboot_v1_0_s_ifspec },
            { Tpm_v1_0_s_ifspec }
        };
        RegisterInterfaces(&rpcSecurityDescriptor, interfaces);
    }
    catch (const DMException)
    {
        TRACELINE(LoggingLevel::Verbose, L"Error: Failed to register interfaces.");
        throw;
    }
    catch (const exception& ex)
    {
        TRACELINEP(
            LoggingLevel::Verbose,
            ex.what(),
            GetLastError());
        throw;
    }
}

shared_ptr<Microsoft::Azure::DeviceManagement::Common::ILocalDMClient> DMBridgeServer::GetLocalDmAccess()
{
    return _localDmAccess;
}

void DMBridgeServer::Listen()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    RPC_STATUS status = RPC_S_OK;

    status = RpcServerListen(
        RPC_MIN_CALLS,
        RPC_C_LISTEN_MAX_CALLS_DEFAULT,
        RPC_DONT_WAIT);

    if (status != RPC_S_OK)
    {
        throw DMException(DMSubsystem::RPC, status, "Failed to listen for RPC");
    }
}

void DMBridgeServer::StopListening()
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    RPC_STATUS status = RPC_S_OK;

    status = RpcMgmtStopServerListening(NULL /* Stop this program's RPC binding*/);

    if (status != RPC_S_OK)
    {
        throw DMException(DMSubsystem::RPC, status, "Failed to stop listening for RPC");
    }
}

void DMBridgeServer::RegisterInterfaces(
    SECURITY_DESCRIPTOR* securityDescriptor,
    const std::vector<RPC_IF_HANDLE>& interfaces)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINEP(LoggingLevel::Verbose, "Number of interfaces: ", static_cast<int>(interfaces.size()));
    RPC_STATUS status = RPC_S_OK;
    for (RPC_IF_HANDLE rpcInterface : interfaces)
    {
        status = RPC_S_OK;
        status = RpcServerRegisterIf3(
            rpcInterface,
            nullptr,
            nullptr,
            RPC_IF_AUTOLISTEN | RPC_IF_ALLOW_LOCAL_ONLY,
            RPC_C_LISTEN_MAX_CALLS_DEFAULT,
            0,
            nullptr,
            securityDescriptor);

        if (status != RPC_S_OK)
        {
            throw DMException(DMSubsystem::RPC, status, "Failed to register interface");
        }
    }
}

SECURITY_DESCRIPTOR DMBridgeServer::GenerateSecurityDescriptor(const WCHAR* capability)
{
    TRACELINE(LoggingLevel::Verbose, __FUNCTION__);
    TRACELINEP(LoggingLevel::Verbose, L"Requiring capability: ", capability);
    // Security Policy
    DWORD hResult = S_OK;
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSID everyoneSid = nullptr;
    PSID* capabilitySids = nullptr;
    DWORD capabilitySidCount = 0;
    PSID* capabilityGroupSids = nullptr;
    DWORD capabilityGroupSidCount = 0;
    EXPLICIT_ACCESS ea[2] = {};
    PACL acl = nullptr;
    SECURITY_DESCRIPTOR rpcSecurityDescriptor = {};

    // Get the SID form of the custom capability.  In this case we only expect one SID and
    // we don't care about the capability group.
    if (!DeriveCapabilitySidsFromName(
        capability,
        &capabilityGroupSids,
        &capabilityGroupSidCount,
        &capabilitySids,
        &capabilitySidCount))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to derive capability sids from name");
    }

    if (capabilitySidCount != 1)
    {
        throw DMException(DMSubsystem::Windows, ERROR_INVALID_PARAMETER, "Unexpected sid count");
    }

    if (!AllocateAndInitializeSid(
        &SIDAuthWorld,
        1,
        SECURITY_WORLD_RID,
        0, 0, 0, 0, 0, 0, 0,
        &everyoneSid))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to allocate and initialize sid");
    }

    // Everyone GENERIC_ALL access
    ea[0].grfAccessMode = SET_ACCESS;
    ea[0].grfAccessPermissions = GENERIC_ALL;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName = static_cast<LPWSTR>(everyoneSid);
    // Custom capability GENERIC_ALL access
    ea[1].grfAccessMode = SET_ACCESS;
    ea[1].grfAccessPermissions = GENERIC_ALL;
    ea[1].grfInheritance = NO_INHERITANCE;
    ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[1].Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
    // Earlier we ensured there was exactly 1
    ea[1].Trustee.ptstrName = static_cast<LPWSTR>(capabilitySids[0]);

    hResult = SetEntriesInAcl(ARRAYSIZE(ea), ea, nullptr, &acl);
    if (hResult != ERROR_SUCCESS)
    {
        throw DMException(DMSubsystem::Windows, hResult, "Failed to set entries in Acl");
    }

    if (!InitializeSecurityDescriptor(&rpcSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to initialize security descriptor");
    }

    if (!SetSecurityDescriptorDacl(&rpcSecurityDescriptor, TRUE, acl, FALSE))
    {
        throw DMException(DMSubsystem::Windows, GetLastError(), "Failed to set security descriptor Dacl");
    }

    return rpcSecurityDescriptor;
}

/******************************************************/
/*         MIDL allocate and free                     */
/******************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}