# ###########################################################################
# Keepalive probe
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# Measures how a connection configured by the framework notices a dead peer.
# keepalive_probe.py drives it and installs the packet drop rules, which needs
# root and takes minutes, so it is built here but not registered with ctest.
# ###########################################################################

set(AREG_KEEPALIVE_PROBE_PROJECT "areg-keepalive-probe")

addExecutableEx(${AREG_KEEPALIVE_PROBE_PROJECT} ""
    "${AREG_TESTS_DIR}/keepalive-probe/keepalive_probe.cpp"
    "")
