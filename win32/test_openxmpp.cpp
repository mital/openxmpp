#pragma once

#include <iostream>

using namespace std;

#include <gtest/gtest.h>

//#include "this/package/foo.h"

// The fixture for testing class Foo.
class FooTest : public ::testing::Test
{
protected:

	FooTest() { }

	virtual ~FooTest() { }

	virtual void SetUp() { }

	virtual void TearDown() { }
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(FooTest, MethodBarDoesAbc) {
	const std::string input_filepath = "this/package/testdata/myinputfile.dat";
	const std::string output_filepath = "this/package/testdata/myoutputfile.dat";
	//Foo f;
	//EXPECT_EQ(0, f.Bar(input_filepath, output_filepath));
}

// Tests that Foo does Xyz.
TEST_F(FooTest, DoesXyz) {
	// Exercises the Xyz feature of Foo.
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}