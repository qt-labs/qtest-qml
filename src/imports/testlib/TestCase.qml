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

    // Internal private state.  Identifiers prefixed with qtest are reserved.
    property bool qtest_prevWhen: true
    property int qtest_testId: -1
    property variant qtest_testCaseResult
    property variant qtest_results: qtest_results_normal
    TestResult { id: qtest_results_normal }
    property variant qtest_events: qtest_events_normal
    TestEvent { id: qtest_events_normal }

    function fail(msg) {
        if (msg === undefined)
            msg = "";
        qtest_results.fail(msg, Qt.qtest_caller_file(), Qt.qtest_caller_line())
        throw new Error("QtQuickTest::fail")
    }

    function qtest_fail(msg, frame) {
        if (msg === undefined)
            msg = "";
        qtest_results.fail(msg, Qt.qtest_caller_file(frame), Qt.qtest_caller_line(frame))
        throw new Error("QtQuickTest::fail")
    }

    function verify(cond, msg) {
        if (msg === undefined)
            msg = "";
        if (!qtest_results.verify(cond, msg, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::fail")
    }

    function qtest_compareInternal(actual, expected) {
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

    function qtest_formatValue(value) {
        if (typeof value == "object") {
            if ("x" in value && "y" in value && "z" in value) {
                return "Qt.vector3d(" + value.x + ", " +
                       value.y + ", " + value.z + ")"
            }
        }
        return value
    }

    function compare(actual, expected, msg) {
        var act = qtest_formatValue(actual)
        var exp = qtest_formatValue(expected)
        var success = qtest_compareInternal(actual, expected)
        if (msg === undefined) {
            if (success)
                msg = "COMPARE()"
            else
                msg = "Compared values are not the same"
        }
        if (!qtest_results.compare(success, msg, act, exp, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::fail")
    }

    function tryCompare(obj, prop, value, timeout) {
        if (!timeout)
            timeout = 5000
        if (!qtest_compareInternal(obj[prop], value))
            wait(0)
        var i = 0
        while (i < timeout && !qtest_compareInternal(obj[prop], value)) {
            wait(50)
            i += 50
        }
        var actual = obj[prop]
        var act = qtest_formatValue(actual)
        var exp = qtest_formatValue(value)
        var success = qtest_compareInternal(actual, value)
        if (!qtest_results.compare(success, "property " + prop, act, exp, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::fail")
    }

    function skip(msg) {
        if (msg === undefined)
            msg = ""
        qtest_results.skipSingle(msg, Qt.qtest_caller_file(), Qt.qtest_caller_line())
        throw new Error("QtQuickTest::skip")
    }

    function skipAll(msg) {
        if (msg === undefined)
            msg = ""
        qtest_results.skipAll(msg, Qt.qtest_caller_file(), Qt.qtest_caller_line())
        throw new Error("QtQuickTest::skip")
    }

    function expectFail(tag, msg) {
        if (tag === undefined) {
            warn("tag argument missing from expectFail()")
            tag = ""
        }
        if (msg === undefined) {
            warn("message argument missing from expectFail()")
            msg = ""
        }
        if (!qtest_results.expectFail(tag, msg, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::expectFail")
    }

    function expectFailContinue(tag, msg) {
        if (tag === undefined) {
            warn("tag argument missing from expectFailContinue()")
            tag = ""
        }
        if (msg === undefined) {
            warn("message argument missing from expectFailContinue()")
            msg = ""
        }
        if (!qtest_results.expectFailContinue(tag, msg, Qt.qtest_caller_file(), Qt.qtest_caller_line()))
            throw new Error("QtQuickTest::expectFail")
    }

    function warn(msg) {
        if (msg === undefined)
            msg = ""
        qtest_results.warn(msg);
    }

    function ignoreWarning(msg) {
        if (msg === undefined)
            msg = ""
        qtest_results.ignoreWarning(msg)
    }

    function wait(ms) {
        qtest_results.wait(ms)
    }

    function sleep(ms) {
        qtest_results.sleep(ms)
    }

    function keyPress(key, modifiers, delay) {
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.keyPress(key, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function keyRelease(key, modifiers, delay) {
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.keyRelease(key, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function keyClick(key, modifiers, delay) {
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.keyClick(key, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function mousePress(item, x, y, button, modifiers, delay) {
        if (button === undefined)
            button = Qt.LeftButton
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.mousePress(item, x, y, button, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function mouseRelease(item, x, y, button, modifiers, delay) {
        if (button === undefined)
            button = Qt.LeftButton
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.mouseRelease(item, x, y, button, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function mouseClick(item, x, y, button, modifiers, delay) {
        if (button === undefined)
            button = Qt.LeftButton
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.mouseClick(item, x, y, button, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function mouseDoubleClick(item, x, y, button, modifiers, delay) {
        if (button === undefined)
            button = Qt.LeftButton
        if (modifiers === undefined)
            modifiers = Qt.NoModifier
        if (delay == undefined)
            delay = -1
        if (!qtest_events.mouseDoubleClick(item, x, y, button, modifiers, delay))
            qtest_fail("window not shown", 2)
    }

    function mouseMove(item, x, y, delay) {
        if (delay == undefined)
            delay = -1
        if (!qtest_events.mouseMove(item, x, y, delay))
            qtest_fail("window not shown", 2)
    }

    // Functions that can be overridden in subclasses for init/cleanup duties.
    function initTestCase() {}
    function cleanupTestCase() {}
    function init() {}
    function cleanup() {}

    function qtest_runInternal(prop, arg) {
        try {
            qtest_testCaseResult = testCase[prop](arg)
        } catch (e) {
            qtest_testCaseResult = []
            if (e.message.indexOf("QtQuickTest::") != 0) {
                // Test threw an unrecognized exception - fail.
                qtest_results.fail("Uncaught exception: " + e.message,
                             e.fileName, e.lineNumber)
            }
        }
        return !qtest_results.dataFailed
    }

    function qtest_runFunction(prop, arg) {
        qtest_results.functionType = TestResult.InitFunc
        qtest_runInternal("init")
        if (!qtest_results.skipped) {
            qtest_results.functionType = TestResult.Func
            qtest_runInternal(prop, arg)
            qtest_results.functionType = TestResult.CleanupFunc
            qtest_runInternal("cleanup")
        }
        qtest_results.functionType = TestResult.NoWhere
    }

    function qtest_runBenchmarkFunction(prop, arg) {
        qtest_results.startMeasurement()
        do {
            qtest_results.beginDataRun()
            do {
                // Run the initialization function.
                qtest_results.functionType = TestResult.InitFunc
                qtest_runInternal("init")
                if (qtest_results.skipped)
                    break

                // Execute the benchmark function.
                qtest_results.functionType = TestResult.Func
                if (prop.indexOf("benchmark_once_") != 0)
                    qtest_results.startBenchmark(TestResult.RepeatUntilValidMeasurement, qtest_results.dataTag)
                else
                    qtest_results.startBenchmark(TestResult.RunOnce, qtest_results.dataTag)
                while (!qtest_results.isBenchmarkDone()) {
                    if (!qtest_runInternal(prop, arg))
                        break
                    qtest_results.nextBenchmark()
                }
                qtest_results.stopBenchmark()

                // Run the cleanup function.
                qtest_results.functionType = TestResult.CleanupFunc
                qtest_runInternal("cleanup")
                qtest_results.functionType = TestResult.NoWhere
            } while (!qtest_results.measurementAccepted())
            qtest_results.endDataRun()
        } while (qtest_results.needsMoreMeasurements())
    }

    function qtest_run() {
        if (TestLogger.log_start_test()) {
            qtest_results.reset()
            qtest_results.testCaseName = name
            qtest_results.startLogging()
        } else {
            qtest_results.testCaseName = name
        }
        running = true

        // Run the initTestCase function.
        qtest_results.functionName = "initTestCase"
        qtest_results.functionType = TestResult.InitFunc
        var runTests = true
        if (!qtest_runInternal("initTestCase"))
            runTests = false
        qtest_results.finishTestFunction()

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
            qtest_results.functionName = prop
            if (datafunc in testCase) {
                qtest_results.functionType = TestResult.DataFunc
                if (qtest_runInternal(datafunc)) {
                    var table = qtest_testCaseResult
                    var haveData = false
                    qtest_results.initTestTable()
                    for (var index in table) {
                        haveData = true
                        var row = table[index]
                        if (!row.tag)
                            row.tag = "row " + index    // Must have something
                        qtest_results.dataTag = row.tag
                        if (isBenchmark)
                            qtest_runBenchmarkFunction(prop, row)
                        else
                            qtest_runFunction(prop, row)
                        qtest_results.dataTag = ""
                    }
                    if (!haveData)
                        qtest_results.warn("no data supplied for " + prop + "() by " + datafunc + "()")
                    qtest_results.clearTestTable()
                }
            } else if (isBenchmark) {
                qtest_runBenchmarkFunction(prop, null, isBenchmark)
            } else {
                qtest_runFunction(prop, null, isBenchmark)
            }
            qtest_results.finishTestFunction()
            qtest_results.skipped = false
        }

        // Run the cleanupTestCase function.
        qtest_results.skipped = false
        qtest_results.functionName = "cleanupTestCase"
        qtest_results.functionType = TestResult.CleanupFunc
        qtest_runInternal("cleanupTestCase")

        // Clean up and exit.
        running = false
        completed = true
        qtest_results.finishTestFunction()
        qtest_results.functionName = ""

        // Stop if there are no more tests to be run.
        if (!TestLogger.log_complete_test(qtest_testId)) {
            qtest_results.stopLogging()
            Qt.quit()
        }
        qtest_results.testCaseName = ""
    }

    onWhenChanged: {
        if (when != qtest_prevWhen) {
            qtest_prevWhen = when
            if (when && !completed && !running)
                qtest_run()
        }
    }

    onOptionalChanged: {
        if (!completed) {
            if (optional)
                TestLogger.log_optional_test(qtest_testId)
            else
                TestLogger.log_mandatory_test(qtest_testId)
        }
    }

    // The test framework will set qtest.windowShown when the
    // window is actually shown.  If we are running with qmlviewer,
    // then this won't happen.  So we use a timer instead.
    Timer {
        id: qtest_windowShowTimer
        interval: 100
        repeat: false
        onTriggered: { windowShown = true }
    }

    Component.onCompleted: {
        qtest_testId = TestLogger.log_register_test(name)
        if (optional)
            TestLogger.log_optional_test(qtest_testId)
        qtest_prevWhen = when
        var isQmlViewer = Qt.qtest_wrapper ? false : true
        if (isQmlViewer)
            qtest_windowShowTimer.running = true
        if (when && !completed && !running)
            qtest_run()
    }
}
