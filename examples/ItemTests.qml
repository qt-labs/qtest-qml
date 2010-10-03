import Qt 4.7
import QtTest 1.0

Rectangle {
    id: foo
    width: 640; height: 480
    color: "cyan"

    MouseArea {
        id: area
        anchors.fill: parent
    }

    property bool bar: true

    TestCase {
        name: "ItemTests"
        when: area.pressed
        id: test1

        function test_bar() {
            verify(bar)
        }
    }

    TestCase {
        when: test1.completed

        function test_bar2() {
            verify(bar)
        }
    }

    TestCase {
        when: false
        optional: true

        function test_not_run() {
            verify(false)
        }
    }
}
