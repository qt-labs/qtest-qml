import Qt 4.7
import QtTest 1.0

TestCase {
    name: "BasicTests"

    function test_pass() {
        compare(2 + 2, 4, "2 + 2")
    }

    function test_fail() {
        compare(2 + 2, 5, "2 + 2")
    }

    function test_skip() {
        skip("skipping")
    }

    function test_expecting() {
        expectFail("this is the fail we wanted")
        verify(false)
    }
}
