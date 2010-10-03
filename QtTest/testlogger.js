
.pragma library

// We need a global place to store the results that can be
// shared between multiple TestCase instances.  Because QML
// creates a separate scope for every inclusion of this file,
// we hijack the global "Qt" object to store our data.
function log_init_results()
{
    if (!Qt.testResults) {
        Qt.testResults = {
            suiteName: "",
            reportedStart: false,
            numPassed: 0,
            numFailed: 0,
            numSkipped: 0,
            nextId: 0,
            testCases: []
        }
    }
}

function log_fail(testcase, msg)
{
    if (!msg)
        msg = ""
    console.log("FAIL!  : " + testcase + " " + msg)
    ++Qt.testResults.numFailed
}

function log_expect_fail(testcase, expectmsg, msg)
{
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
    console.log("XPASS  : " + testcase)
    ++Qt.testResults.numFailed
}

function log_skip(testcase, msg)
{
    if (!msg)
        msg = ""
    console.log("SKIP   : " + testcase + " " + msg)
    ++Qt.testResults.numSkipped
}

function log_pass(testcase)
{
    console.log("PASS   : " + testcase)
    ++Qt.testResults.numPassed
}

function log_message(msg)
{
    console.log(msg)
}

function log_print_totals()
{
    console.log("Totals: " + Qt.testResults.numPassed + " passed, " +
                             Qt.testResults.numFailed + " failed, " +
                             Qt.testResults.numSkipped + " skipped");
}

function log_register_test(name)
{
    log_init_results()
    if (name && !Qt.testResults.suiteName)
        Qt.testResults.suiteName = name
    var testId = Qt.testResults.nextId++
    Qt.testResults.testCases.push(testId)
    return testId
}

function log_optional_test(testId)
{
    log_init_results()
    var index = Qt.testResults.testCases.indexOf(testId)
    if (index >= 0)
        Qt.testResults.testCases.splice(index, 1)
}

function log_mandatory_test(testId)
{
    log_init_results()
    var index = Qt.testResults.testCases.indexOf(testId)
    if (index == -1)
        Qt.testResults.testCases.push(testId)
}

function log_start_test()
{
    log_init_results()
    if (Qt.testResults.reportedStart)
        return
    if (Qt.testResults.suiteName)
        console.log("********* Start testing of " + Qt.testResults.suiteName + " *********")
    else
        console.log("********* Start testing *********")
    Qt.testResults.reportedStart = true
}

function log_complete_test(testId)
{
    var index = Qt.testResults.testCases.indexOf(testId)
    if (index >= 0)
        Qt.testResults.testCases.splice(index, 1)
    if (!Qt.testResults.testCases.length) {
        log_print_totals()
        if (Qt.testResults.suiteName)
            console.log("********* Finished testing of " + Qt.testResults.suiteName + " *********")
        else
            console.log("********* Finished testing *********")
        Qt.quit()       // XXX - how do we set the exit value?
    }
}

function log_prefixed_name(name, funcname)
{
    if (!name)
        name = Qt.testResults.suiteName
    if (name)
        return name + "::" + funcname + "()"
    else
        return funcname + "()"
}
