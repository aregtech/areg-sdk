# ###########################################################################
# 32-bit and 64-bit interoperation test
# Copyright 2022-2026 Aregtech (Artak Avetyan)
# ###########################################################################
#
# A provider and a consumer that exchange one sample of every data kind as a request,
# a response, an attribute and a broadcast, and report each field that arrives different.
# bitness.sh (bitness.ps1 on Windows) runs them from a 32-bit and a 64-bit build through one
# router. It starts a router, so it is not registered with ctest.
# ###########################################################################

set(AREG_BITNESS_PROJECT "areg-bitness-test")

addServiceInterface(bitness_generated tests/bitness/services/BitnessCheck.siml)

addExecutableEx(${AREG_BITNESS_PROJECT} ""
    "${AREG_TESTS_DIR}/bitness/bitness_test.cpp"
    "bitness_generated")
