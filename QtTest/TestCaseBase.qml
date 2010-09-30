import Qt 4.7

Item {
    id: testCase
    visible: false
    property string name
    property variant tests: []

    property string currentTestCase
    property int numPassed
    property int numFailed
    property int numSkipped
    property bool expectingFail
    property string expectFailMsg

    function fail(msg) {
        if (!msg)
            msg = "";
        if (expectingFail) {
            if (expectFailMsg)
                print("XFAIL  : " + currentTestCase + " " + expectFailMsg + " " + msg)
            else
                print("XFAIL  : " + currentTestCase + " " + msg)
            throw new Error("QtTest::expect_fail")
        } else {
            print("FAIL!  : " + currentTestCase + " " + msg)
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
        print("SKIP   : " + currentTestCase + " " + msg)
        throw new Error("QtTest::skip")
    }

    function expectFail(msg) {
        expectingFail = true
        expectFailMsg = msg
    }

    property variant testCaseResult

    function runInternal(prop, dataDriven, arg, tag) {
        var prefix;
        if (name)
            prefix = name + "::"
        if (dataDriven && tag)
            currentTestCase = prefix + prop + "() [" + tag + "]"
        else
            currentTestCase = prefix + prop + "()"
        expectingFail = false
        var success = true
        try {
            testCaseResult = testCase[prop](arg)
            if (expectingFail) {
                ++numFailed
                success = false
                print("XPASS  : " + currentTestCase)
            } else if (!dataDriven) {
                print("PASS   : " + currentTestCase)
            }
        } catch (e) {
            testCaseResult = []
            if (e.message == "QtTest::fail") {
                ++numFailed
                success = false
            } else if (e.message == "QtTest::skip") {
                ++numSkipped
            } else if (e.message == "QtTest::expect_fail") {
                ++numFailed
            }
        }
        return success
    }

    function run() {
        var success = true
        numPassed = 0
        numFailed = 0
        numSkipped = 0
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
                        print("WARNING: no data supplied for " + prop + "() by " + datafunc + "()")
                    if (successThis) {
                        var prefix;
                        if (name)
                            prefix = name + "::"
                        currentTestCase = prefix + prop + "()"
                        print("PASS   : " + currentTestCase)
                        ++numPassed
                    } else {
                        success = false
                    }
                } else {
                    success = false
                }
            } else {
                if (runInternal(prop, false))
                    ++numPassed
                else
                    success = false
            }
        }
        currentTestCase = ""
        return success;
    }
}
