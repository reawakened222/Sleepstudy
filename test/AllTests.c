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

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		int nrOfFailedTests = RunAllTests();

		return (nrOfFailedTests == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
	}
	else
	{
		/* Soak mode */
		int nrOfTestExecutionFailures = 0;
		int nrOfSoakRuns = atoi(argv[1]);

		for (int i = 0; i < nrOfSoakRuns; i++)
		{
			nrOfTestExecutionFailures += (RunAllTests() == 0) ? 0 : 1;
		}
		printf("Nr of Test Executions: %d\n", nrOfSoakRuns);
		printf("Nr of Failed Test Executions: %d\n", nrOfTestExecutionFailures);
		printf("Flakiness: %f%%\n", ((float)nrOfTestExecutionFailures / nrOfSoakRuns) * 100);
	}
}
