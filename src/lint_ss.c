/*
 * GS1 Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file lint_ss.c
 *
 * @brief The `ss` linter ensures that the given data is a meaningful second
 * number in a minute.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to SS format for seconds within
 * a minute.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_SECOND_TOO_SHORT if the data is too short for MM format.
 * @return #GS1_LINTER_SECOND_TOO_LONG if the data is too long for MM format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_SECOND if the data contains an invalid second.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_ss(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t len, pos;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be two characters.
	 *
	 */
	if (len != 2) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return len < 2 ? GS1_LINTER_SECOND_TOO_SHORT : GS1_LINTER_SECOND_TOO_LONG;
	}

	/*
	 * Data must consist of all digits.
	 *
	 */
	if ((pos = strspn(data, "0123456789")) != len) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_NON_DIGIT_CHARACTER;
	}

	/*
	 * Validate the second.
	 *
	 */
	if ((data[0] - '0') * 10 + (data[1] - '0') > 59) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_SECOND;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_ss(void)
{

	UNIT_TEST_PASS(gs1_lint_ss, "00");
	UNIT_TEST_PASS(gs1_lint_ss, "59");
	UNIT_TEST_FAIL(gs1_lint_ss, "60",  GS1_LINTER_ILLEGAL_SECOND, "*60*");

	UNIT_TEST_FAIL(gs1_lint_ss, "x0",  GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0");
	UNIT_TEST_FAIL(gs1_lint_ss, "0x",  GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*");

	UNIT_TEST_FAIL(gs1_lint_ss, "",    GS1_LINTER_SECOND_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_ss, "1",   GS1_LINTER_SECOND_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_ss, "111", GS1_LINTER_SECOND_TOO_LONG,  "*111*");

}

#endif  /* UNIT_TESTS */
