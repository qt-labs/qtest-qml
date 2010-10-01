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

        function test_bar() {
            verify(bar)
        }
    }
}
