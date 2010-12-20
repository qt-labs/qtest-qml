import Qt 4.7
import QtQuickTest 1.0

TestCase {
    name: "SelfTests_CompareObjects"

    function test_qtest_internalCompareObjects() {
        var testObj11 = {a: 1, b: "foo", c: true};
        var testObj12 = {a: 1, b: "foo", c: true};
        var testObj21 = {a: 1, b: "foo", c: true, d: testObj11};
        var testObj22 = {a: 1, b: "foo", c: true, d: testObj11};
        var testObj23 = {a: 1, b: "foo", c: true, d: testObj12};

        compare(qtest_compareInternalObjects(testObj11, testObj11), true, "Object identity");
        compare(qtest_compareInternalObjects(testObj11, testObj12), true, "Object equality");
        compare(qtest_compareInternalObjects(testObj21, testObj21), true, "Nested object identity");
        compare(qtest_compareInternalObjects(testObj21, testObj22), true, "Nested object equality");
        compare(qtest_compareInternalObjects(testObj21, testObj23), true, "Deep nested object equality");
    }

    function test_qtest_internalCompareObjects_differentObjects() {
        var testObj11 = {a: 1, b: "foo", c: true};
        var testObj12 = {a: 2, b: "foo", c: true};
        var testObj13 = {a: 1, b: "bar", c: true};
        var testObj14 = {a: 1, b: "foo", c: false};

        compare(qtest_compareInternalObjects(testObj11, testObj12), false, "Different number");
        compare(qtest_compareInternalObjects(testObj11, testObj13), false, "Different string");
        compare(qtest_compareInternalObjects(testObj11, testObj14), false, "Different boolean");
    }


}
