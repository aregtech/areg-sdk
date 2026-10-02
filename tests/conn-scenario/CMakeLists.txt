# ###########################################################################
# Connection scenario harness
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# A provider and a consumer that report sequence gaps, stalls and damaged blocks
# while conn_scenario.py pauses a process or drops packets. Reuses the generated
# service interfaces of examples 25 and 32, so it is built only when the examples are built.
# It starts a router and takes minutes, so it is not registered with ctest.
# ###########################################################################

set(AREG_CONN_SCENARIO_PROJECT "areg-conn-scenario")

addExecutableEx(${AREG_CONN_SCENARIO_PROJECT} ""
    "${AREG_TESTS_DIR}/conn-scenario/conn_scenario.cpp;${AREG_TESTS_DIR}/conn-scenario/scenario_provider.cpp;${AREG_TESTS_DIR}/conn-scenario/scenario_consumer.cpp"
    "32_generated;25_generated")
