/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 1.0
import QtQuickTest 1.0
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

    // Property that is set to true when the main window is shown.
    // We need to set the property value in an odd way to handle
    // both qmlviewer and the QtQuickTest module test wrapper.
    property bool windowShown: Qt.qtest_wrapper ? qtest.windowShown : false

    // Internal private state
    property bool prevWhen: true
    property int testId: -1
    property variant testCaseResult

    TestResult { id: results }

    function fail(msg) {
        if (!msg)
            msg = "";
        results.fail(msg, Qt.qtest_caller_file(), Qt.qtest_caller_line())
        throw new Error("QtQuickTest::fail")
    }

    function verify(cond, msg) {
        if (!msg)
            msg = "";
        if (!results.verify(cond, msg, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::fail")
    }

    function compareInternal(actual, expected) {
        var success = false
        if (typeof actual == "number" && typeof expected == "number") {
            // Use a fuzzy compare if the two values are floats
            if (Math.abs(actual - expected) <= 0.00001)
                success = true
        } else if (typeof actual == "object" && typeof expected == "object") {
            // Does the expected value look like a vector3d?
            if ("x" in expected && "y" in expected && "z" in expected) {
                if (Math.abs(actual.x - expected.x) <= 0.00001 &&
                        Math.abs(actual.y - expected.y) <= 0.00001 &&
                        Math.abs(actual.z - expected.z) <= 0.00001) {
                    success = true
                }
            } else if (actual == expected) {
                success = true
            }
        } else if (actual == expected) {
            success = true
        }
        return success
    }

    function formatValue(value) {
        if (typeof value == "object") {
            if ("x" in value && "y" in value && "z" in value) {
                return "Qt.vector3d(" + value.x + ", " +
                       value.y + ", " + value.z + ")"
            }
        }
        return value
    }

    function compare(actual, expected, msg) {
        var act = formatValue(actual)
        var exp = formatValue(expected)
        var success = compareInternal(actual, expected)
        if (!msg)
            msg = ""
        if (!results.compare(success, msg, act, exp, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::fail")
    }

    function tryCompare(obj, prop, value, timeout) {
        if (!timeout)
            timeout = 5000
        if (!compareInternal(obj[prop], value))
            wait(0)
        var i = 0
        while (i < timeout && !compareInternal(obj[prop], value)) {
            wait(50)
            i += 50
        }
        var actual = obj[prop]
        var act = formatValue(actual)
        var exp = formatValue(value)
        var success = compareInternal(actual, value)
        if (!results.compare(success, "property " + prop, act, exp, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::fail")
    }

    function skip(msg) {
        if (!msg)
            msg = ""
        results.skipSingle(msg, Qt.qtest_caller_file(), Qt.qtest_caller_line())
        throw new Error("QtQuickTest::skip")
    }

    function skipAll(msg) {
        if (!msg)
            msg = ""
        results.skipAll(msg, Qt.qtest_caller_file(), Qt.qtest_caller_line())
        throw new Error("QtQuickTest::skip")
    }

    function expectFail(tag, msg) {
        if (!tag)
            tag = ""
        if (!msg)
            msg = ""
        if (!results.expectFail(tag, msg, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::expectFail")
    }

    function expectFailContinue(tag, msg) {
        if (!tag)
            tag = ""
        if (!msg)
            msg = ""
        if (!results.expectFailContinue(tag, msg, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::expectFail")
    }

    function warn(msg) {
        if (!msg)
            msg = ""
        results.warn(msg);
    }

    function ignoreWarning(msg) {
        if (!msg)
            msg = ""
        results.ignoreWarning(msg)
    }

    function wait(ms) {
        results.wait(ms)
    }

    function sleep(ms) {
        results.sleep(ms)
    }

    // Functions that can be overridden in subclasses for init/cleanup duties.
    function initTestCase() {}
    function cleanupTestCase() {}
    function init() {}
    function cleanup() {}

    function runInternal(prop, arg) {
        try {
            testCaseResult = testCase[prop](arg)
        } catch (e) {
            testCaseResult = []
            if (e.message.indexOf("QtQuickTest::") != 0) {
                // Test threw an unrecognized exception - fail.
                fail(e.message)
            }
        }
        return !results.dataFailed
    }

    function runFunction(prop, arg) {
        results.functionType = TestResult.InitFunc
        runInternal("init")
        if (!results.skipped) {
            results.functionType = TestResult.Func
            runInternal(prop, arg)
            results.functionType = TestResult.CleanupFunc
            runInternal("cleanup")
        }
        results.functionType = TestResult.NoWhere
    }

    function runBenchmarkFunction(prop, arg) {
        results.startMeasurement()
        do {
            results.beginDataRun()
            do {
                // Run the initialization function.
                results.functionType = TestResult.InitFunc
                runInternal("init")
                if (results.skipped)
                    break

                // Execute the benchmark function.
                results.functionType = TestResult.Func
                if (prop.indexOf("benchmark_once_") != 0)
                    results.startBenchmark(TestResult.RepeatUntilValidMeasurement, results.dataTag)
                else
                    results.startBenchmark(TestResult.RunOnce, results.dataTag)
                while (!results.isBenchmarkDone()) {
                    if (!runInternal(prop, arg))
                        break
                    results.nextBenchmark()
                }
                results.stopBenchmark()

                // Run the cleanup function.
                results.functionType = TestResult.CleanupFunc
                runInternal("cleanup")
                results.functionType = TestResult.NoWhere
            } while (!results.measurementAccepted())
            results.endDataRun()
        } while (results.needsMoreMeasurements())
    }

    function run() {
        if (TestLogger.log_start_test()) {
            results.reset()
            results.testCaseName = name
            results.startLogging()
        } else {
            results.testCaseName = name
        }
        running = true

        // Run the initTestCase function.
        results.functionName = "initTestCase"
        results.functionType = TestResult.InitFunc
        var runTests = true
        if (!runInternal("initTestCase"))
            runTests = false
        results.finishTestFunction()

        // Run the test methods.
        var testList = []
        if (runTests) {
            for (var prop in testCase) {
                if (prop.indexOf("test_") != 0 && prop.indexOf("benchmark_") != 0)
                    continue
                var tail = prop.lastIndexOf("_data");
                if (tail != -1 && tail == (prop.length - 5))
                    continue
                testList.push(prop)
            }
            testList.sort()
        }
        for (var index in testList) {
            var prop = testList[index]
            var datafunc = prop + "_data"
            var isBenchmark = (prop.indexOf("benchmark_") == 0)
            results.functionName = prop
            if (datafunc in testCase) {
                results.functionType = TestResult.DataFunc
                if (runInternal(datafunc)) {
                    var table = testCaseResult
                    var haveData = false
                    results.initTestTable()
                    for (var index in table) {
                        haveData = true
                        var row = table[index]
                        if (!row.tag)
                            row.tag = "row " + index    // Must have something
                        results.dataTag = row.tag
                        if (isBenchmark)
                            runBenchmarkFunction(prop, row)
                        else
                            runFunction(prop, row)
                        results.dataTag = ""
                    }
                    if (!haveData)
                        results.warn("no data supplied for " + prop + "() by " + datafunc + "()")
                    results.clearTestTable()
                }
            } else if (isBenchmark) {
                runBenchmarkFunction(prop, null, isBenchmark)
            } else {
                runFunction(prop, null, isBenchmark)
            }
            results.finishTestFunction()
            results.skipped = false
        }

        // Run the cleanupTestCase function.
        results.skipped = false
        results.functionName = "cleanupTestCase"
        results.functionType = TestResult.CleanupFunc
        runInternal("cleanupTestCase")

        // Clean up and exit.
        running = false
        completed = true
        results.finishTestFunction()
        results.functionName = ""

        // Stop if there are no more tests to be run.
        if (!TestLogger.log_complete_test(testId)) {
            results.stopLogging()
            Qt.quit()
        }
        results.testCaseName = ""
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

    // The test framework will set qtest.windowShown when the
    // window is actually shown.  If we are running with qmlviewer,
    // then this won't happen.  So we use a timer instead.
    Timer {
        id: windowShowTimer
        interval: 100
        repeat: false
        onTriggered: { windowShown = true }
    }

    Component.onCompleted: {
        testId = TestLogger.log_register_test(name)
        if (optional)
            TestLogger.log_optional_test(testId)
        prevWhen = when
        var isQmlViewer = Qt.qtest_wrapper ? false : true
        if (isQmlViewer)
            windowShowTimer.running = true
        if (when && !completed && !running)
            run()
    }
}
