import Qt 4.7
import QtQuickTest 1.0

TestCase {
    function test_compare_objects() {
        var testObj1 = {a: 1, b: "foo", c: true};
        compare(testObj1, testObj1, "Object identity");

        var testObj2 = {a: 1, b: "foo", c: true};
        compare(testObj1, testObj2, "Object equality");
    }
}
