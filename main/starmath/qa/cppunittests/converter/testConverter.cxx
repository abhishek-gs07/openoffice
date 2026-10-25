/**************************************************************
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 *************************************************************/

#include <testshl/simpleheader.hxx>

#include "converter.hxx"

class TestConverter : public CppUnit::TestFixture
{
public:
    void testCommandLookupAndUnknownCommands()
    {
        SmConverter::CommandTable aCommands;
        aCommands[String::CreateFromAscii("known")]
            = String::CreateFromAscii("mapped");

        const String aResult = SmConverter::Convert(
            String::CreateFromAscii("\\known+\\unknown"), aCommands);
        CPPUNIT_ASSERT(aResult == String::CreateFromAscii("mapped+\\unknown"));
    }

    void testGroupsAndScripts()
    {
        SmConverter::CommandTable aCommands;
        aCommands[String::CreateFromAscii("alpha")]
            = String::CreateFromAscii("alpha");

        const String aResult = SmConverter::Convert(
            String::CreateFromAscii("x^{\\alpha}_{i}"), aCommands);
        CPPUNIT_ASSERT(aResult == String::CreateFromAscii("x^{alpha}_{i}"));
    }

    void testFractionArguments()
    {
        const String aResult = SmConverter::ConvertLatex(
            String::CreateFromAscii("\\frac {a+b} {c}"));
        CPPUNIT_ASSERT(aResult == String::CreateFromAscii("{a+b} over {c}"));
    }

    void testNestedFractions()
    {
        const String aResult = SmConverter::ConvertLatex(
            String::CreateFromAscii("\\frac{\\frac{a}{b}}{c}"));
        CPPUNIT_ASSERT(aResult == String::CreateFromAscii("{{a} over {b}} over {c}"));
    }

    CPPUNIT_TEST_SUITE(TestConverter);
    CPPUNIT_TEST(testCommandLookupAndUnknownCommands);
    CPPUNIT_TEST(testGroupsAndScripts);
    CPPUNIT_TEST(testFractionArguments);
    CPPUNIT_TEST(testNestedFractions);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestConverter, "TestConverter");
NOADDITIONAL;