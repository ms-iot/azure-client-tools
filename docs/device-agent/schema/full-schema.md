# 10. Full Schema

[Schema](../schema.md)

### Sample

<pre>
{
    "properties": {
        "desired": {
            "__portalSchemas_1": {
                "rebootInfo_2": {
                    "maxInputVersion": "2.0",
                    "outputVersion": "2.0"
                },
                "apps_2": {
                    "maxInputVersion": "2.0",
                    "outputVersion": "2.0"
                }
            },
            "__deviceSchemas_1": {
                "reporting_2": {
                    "groups": "all|configured;&lt;group1&gt;;&lt;group2&gt;",
                    "types": "all|raw",
                    "tags": "&lt;tag1&gt;;&lt;tag2&gt;"
                }
            },
            "rebootInfo_1": {
                "__meta_2": {
                    "policy": "local|remote",
                    "dependencies": "azureStorage",
                    "deployment": "RS4",
                    "reporting_3": {
                        "report": true|false,
                        "details": "all|minimal"
                    }
                },
                "dailyRebootTime": "<i>time</i>",
                "singleRebootTime": "<i>time</i>"
            },
            "apps_1": {
                "__meta_2": {
                    "policy": "local|remote",
                    "dependencies": "azureStorage"
                },
                "uiApp_2": {
                    "__meta_3": {
                        "deployment": "RS4",
                        "reporting_4": {
                            "report": true|false,
                            "details": "all|minimal"
                        }
                    },
                    "appx": "uiApp.appx",
                    "container": "myAppsStorage",
                    "startUp": true
                },
                "bkApp_2": {
                    "__meta_3": {
                        "deployment": "RS4",
                        "reporting_4": {
                            "report": true|false,
                            "details": "all|minimal"
                        }
                    },
                    "appx": "bkApp.appx",
                    "container": "myAppsStorage"
                    "startUp": true
                }
            }
        },
        "reported": {
            "__summary_1": { 
                "failedCount": &lt;count&gt;,
                "failedGroups": "apps:appXYZ;appABC|windowsUpdate",
                "pendingCount": &lt;count&gt;,
                "pendingGroups": "apps:appXYZ;appABC|windowsUpdate",
                "deployments": "&lt;deployment1_id;deployment2_id&gt;"
                "time": "&lt;time&gt;"
            },
            "__deviceSchemas_1": {
                "rebootInfo_2": {
                    "type": "raw",
                    "tags": "dm;sensor",
                    "maxInputVersion": "2.0",
                    "outputVersion": "3.0",
                },
                "apps_2": {
                    "type": "raw",
                    "tags": "dm;sensor",
                    "maxInputVersion": "2.0",
                    "outputVersion": "2.0"
                }
            },
            "rebootInfo_1": {
                "dailyRebootTime": "<i>time</i>",
                "singleRebootTime": "<i>time</i>",
                "__meta_2": {
                    "deployment": "RS4",
                    "time": "__time__",
                    "state": "succeeded|pending|failed"
                },
                "__errors_2": {
                    "dailyRebootTime_3": {
                        "subsystem": "os",
                        "context": "set",
                        "code": 123,
                        "parameters_5": {}
                    },
                    "singleRebootTime_3": {
                        "subsystem": "os",
                        "context": "set",
                        "code": 123,
                        "parameters_4": {}
                    }
                }
            },
            "apps_1": {
                "uiApp_2": {
                    "appx": "uiApp.appx",
                    "container": "myAppsStorage",
                    "startUp": true,
                    "__meta_3": {
                        "deployment": "RS4",
                        "time": "__time__",
                        "state": "succeeded|pending|failed"
                    },
                    "__errors_3": {
                        "install_4": {
                            "subsystem": "os",
                            "context": "installingPackage",
                            "code": 123,
                            "parameters_5": {
                                "fileName": "uiApp.appx"
                            }
                        }
                    }
                }
                },
                "bkApp_2": {
                    "appx": "bkApp.appx",
                    "container": "myAppsStorage",
                    "startUp": true,
                    "__meta_3": {
                        "deployment": "RS4",
                        "time": "__time__",
                        "state": "succeeded|pending|failed"
                    },
                    "__errors_3": null
                }
            }
        }
    }
}
</pre>

----

Last Updated 9/6/2018

[Schema](../schema.md)