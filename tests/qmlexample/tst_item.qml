import Qt 4.7
import QtTest 1.0

Rectangle {
    id: foo
    width: 640; height: 480
    color: "cyan"

    TestCase {
        name: "ItemTests"
        id: test1

        function test_color() {
            compare(foo.color, "#00ffff")
        }
    }
}
