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
        if (actual != expected)
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

    function run() {
        var prefix;
        if (name)
            prefix = name + "::"
        var success = true
        numPassed = 0
        numFailed = 0
        numSkipped = 0
        for (var prop in testCase) {
            if (prop.indexOf("test_") != 0)
                continue
            currentTestCase = prefix + prop + "()"
            expectingFail = false
            try {
                testCase[prop]()
                if (expectingFail) {
                    ++numFailed
                    print("XPASS  : " + currentTestCase)
                } else {
                    ++numPassed
                    print("PASS   : " + currentTestCase)
                }
            } catch (e) {
                if (e.message == "QtTest::fail") {
                    ++numFailed
                    success = false
                } else if (e.message == "QtTest::skip") {
                    ++numSkipped
                } else if (e.message == "QtTest::expect_fail") {
                    ++numFailed
                }
            }
        }
        currentTestCase = ""
        return success;
    }
}
