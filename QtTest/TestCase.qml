import Qt 4.7
import QtTest 1.0

TestCaseBase {
    Component.onCompleted: {
        prevWhen = when
        if (when && !completed && !running) {
            print("********* Start testing of " + name + " *********")
            var success = run()
            print("Totals: " + numPassed + " passed, " +
                               numFailed + " failed, " +
                               numSkipped + " skipped");
            print("********* Finished testing of " + name + " *********")
            Qt.quit()       // XXX - how do we set the exit value?
        }
    }
}
