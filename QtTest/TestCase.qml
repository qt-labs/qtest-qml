import Qt 4.7
import "testlogger.js" as TestLogger

Item {
    id: testCase
    visible: false

    // Name of the test case to prefix the function name in messages.
    property string name

    // Set to true to start the test running.
    property bool when: true

    // Set to true once the test has completed.
    property bool completed: false

    // Set to true when the test is running but not yet complete.
    property bool running: false

    // Set to true if the test doesn't have to run (because some
    // other test failed which this one depends on).
    property bool optional: false

    // Internal private state
    property string currentTestCase
    property bool expectingFail
    property string expectFailMsg
    property bool prevWhen: true
    property int testId: -1

    function fail(msg) {
        if (!msg)
            msg = "";
        if (expectingFail) {
            TestLogger.log_expect_fail(currentTestCase, expectFailMsg, msg)
            throw new Error("QtTest::expect_fail")
        } else {
            TestLogger.log_fail(currentTestCase, msg)
            throw new Error("QtTest::fail")
        }
    }

    function fail2(msg, msg2) {
        if (msg)
            fail(msg + ": " + msg2)
        else
            fail(msg2)
    }

    function verify(cond, msg) {
        if (!cond)
            fail(msg)
    }

    function compare(actual, expected, msg) {
        if (typeof actual == "number" && typeof expected == "number") {
            // Use a fuzzy compare if the two values are floats
            if (Math.abs(actual - expected) <= 0.00001)
                return;
        } else if (actual == expected) {
            return;
        }
        fail2(msg, "actual: " + actual + ", expected: " + expected)
    }

    function skip(msg) {
        TestLogger.log_skip(currentTestCase, msg)
        throw new Error("QtTest::skip")
    }

    function expectFail(msg) {
        expectingFail = true
        expectFailMsg = msg
    }

    property variant testCaseResult

    function runInternal(prop, dataDriven, arg, tag) {
        currentTestCase = TestLogger.log_prefixed_name(name, prop)
        if (dataDriven && tag)
            currentTestCase += " [" + tag + "]"
        expectingFail = false
        var success = true
        try {
            testCaseResult = testCase[prop](arg)
            if (expectingFail) {
                success = false
                TestLogger.log_expect_fail_pass(currentTestCase)
            } else if (!dataDriven) {
                TestLogger.log_pass(currentTestCase)
            }
        } catch (e) {
            testCaseResult = []
            if (e.message == "QtTest::fail")
                success = false
        }
        return success
    }

    function run() {
        TestLogger.log_start_test()
        var success = true
        running = true
        for (var prop in testCase) {
            if (prop.indexOf("test_") != 0)
                continue
            var tail = prop.lastIndexOf("_data");
            if (tail != -1 && tail == (prop.length - 5))
                continue
            var datafunc = prop + "_data"
            if (datafunc in testCase) {
                if (runInternal(datafunc, true)) {
                    var table = testCaseResult
                    var successThis = true
                    var haveData = false
                    for (var index in table) {
                        haveData = true
                        var row = table[index]
                        if (!runInternal(prop, true, row, row.tag))
                            successThis = false
                    }
                    if (!haveData)
                        TestLogger.log_message("WARNING: no data supplied for " + prop + "() by " + datafunc + "()")
                    if (successThis) {
                        var prefix;
                        if (name)
                            prefix = name + "::"
                        currentTestCase = prefix + prop + "()"
                        TestLogger.log_pass(currentTestCase)
                    } else {
                        success = false
                    }
                } else {
                    success = false
                }
            } else {
                if (!runInternal(prop, false))
                    success = false
            }
        }
        currentTestCase = ""
        running = false
        completed = true
        TestLogger.log_complete_test(testId)
        return success
    }

    onWhenChanged: {
        if (when != prevWhen) {
            prevWhen = when
            if (when && !completed && !running)
                run()
        }
    }

    onOptionalChanged: {
        if (!completed) {
            if (optional)
                TestLogger.log_optional_test(testId)
            else
                TestLogger.log_mandatory_test(testId)
        }
    }

    Component.onCompleted: {
        testId = TestLogger.log_register_test(name)
        if (optional)
            TestLogger.log_optional_test(testId)
        prevWhen = when
        if (when && !completed && !running)
            run()
    }
}
