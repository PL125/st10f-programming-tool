include (${TestFunctions})

set (TestArgs "${TestArgs} ${TestConfigOptions}")

exec_test ("${TestArgs}" ${TestExitCode})
