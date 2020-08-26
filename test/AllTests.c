#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CuTest.h"
#include <assert.h>
#include "testSuites.h"
int RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	/* Setup test suites */
	CuSuite* testSuite = ServerBasicFunctionalitySuite();
	assert(testSuite);
	CuSuiteAddSuite(suite, testSuite);
	CuSuiteRun(suite);

	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
		
	return suite->failCount;
}

int main(void)
{
	int nrOfFailedTests = RunAllTests();

	return (nrOfFailedTests == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
