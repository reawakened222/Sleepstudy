#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "CuTest.h"
int RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	/* Setup test suites */
	CuSuiteAddSuite(suite, ServerBasicFunctionalitySuite());

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
