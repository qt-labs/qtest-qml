
.pragma library

// We need a global place to store the results that can be
// shared between multiple TestCase instances.  Because QML
// creates a separate scope for every inclusion of this file,
// we hijack the global "Qt" object to store our data.
function log_init_results()
{
    if (!Qt.testResults) {
        Qt.testResults = {
            numPassed: 0,
            numFailed: 0,
            numSkipped: 0
        }
    }
}

function log_fail(testcase, msg)
{
    log_init_results()
    if (!msg)
        msg = ""
    console.log("FAIL!  : " + testcase + " " + msg)
    ++Qt.testResults.numFailed
}

function log_expect_fail(testcase, expectmsg, msg)
{
    log_init_results()
    if (!msg)
        msg = ""
    if (expectmsg)
        console.log("XFAIL  : " + testcase + " " + expectmsg + " " + msg)
    else
        console.log("XFAIL  : " + testcase + " " + msg)
    ++Qt.testResults.numPassed
}

function log_expect_fail_pass(testcase)
{
    log_init_results()
    console.log("XPASS  : " + testcase)
    ++Qt.testResults.numFailed
}

function log_skip(testcase, msg)
{
    log_init_results()
    if (!msg)
        msg = ""
    console.log("SKIP   : " + testcase + " " + msg)
    ++Qt.testResults.numSkipped
}

function log_pass(testcase)
{
    log_init_results()
    console.log("PASS   : " + testcase)
    ++Qt.testResults.numPassed
}

function log_print_totals()
{
    log_init_results()
    console.log("Totals: " + Qt.testResults.numPassed + " passed, " +
                             Qt.testResults.numFailed + " failed, " +
                             Qt.testResults.numSkipped + " skipped");
}
