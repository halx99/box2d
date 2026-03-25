// SPDX-FileCopyrightText: 2023 Erin Catto
// SPDX-License-Identifier: MIT

#include "test_macros.h"

#if defined( _MSC_VER )
  #include <inttypes.h>
	#include <crtdbg.h>

// int MyAllocHook(int allocType, void* userData, size_t size, int blockType, long requestNumber, const unsigned char* filename,
//	int lineNumber)
//{
//	if (size == 16416)
//	{
//		size += 0;
//	}
//
//	return 1;
// }
#endif

#if defined(_WIN32) && defined(_M_ARM64)
#include <arm64intr.h>
static void b2FixArm64NeonDeterminism()
{
    // Microsoft official standard method to access FPCR
    uint64_t fpcr = _ReadStatusReg(ARM64_FPCR);

    printf("default fpcr: %llu\n", fpcr);

    // Disable FTZ (bit 19) and DAZ (bit 20) to preserve denormals
    // Required for floating-point determinism on Windows ARM64
    fpcr &= ~(1ULL << 19);
    fpcr &= ~(1ULL << 20);

    // Force IEEE 754 standard rounding mode: Round to Nearest, Ties to Even
    fpcr &= ~(3ULL << 22);
    fpcr |=  (0ULL << 22);

    // UE5 recommended settings for full cross-platform NEON determinism
    // AH (bit 24): Alternate Half
    // NEP (bit 25): NEON Processing mode (unifies scalar/vector precision)
    fpcr |= (1ULL << 24);
    fpcr |= (1ULL << 25);

    printf("new fpcr: %llu", fpcr);
    // Write back the finalized FPCR state
    _WriteStatusReg(ARM64_FPCR, fpcr);
}
#else
static void b2FixArm64NeonDeterminism() {}
#endif

extern int BitSetTest( void );
extern int CollisionTest( void );
extern int DeterminismTest( void );
extern int DistanceTest( void );
extern int DynamicTreeTest( void );
extern int IdTest( void );
extern int MathTest( void );
extern int ShapeTest( void );
extern int TableTest( void );
extern int WorldTest( void );

int main( void )
{
#if defined( _MSC_VER )
	// Enable memory-leak reports

	// How to break at the leaking allocation, in the watch window enter this variable
	// and set it to the allocation number in {}. Do this at the first line in main.
	// {,,ucrtbased.dll}_crtBreakAlloc = <allocation number> 3970
	// Note:
	// Just _crtBreakAlloc in static link
	// Tracy Profile server leaks

	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
	//_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
	//_CrtSetAllocHook(MyAllocHook);
	//_CrtSetBreakAlloc(196);
#endif

  b2FixArm64NeonDeterminism();

	printf( "Starting Box2D unit tests\n" );
	printf( "======================================\n" );

	RUN_TEST( TableTest );
	RUN_TEST( MathTest );
	RUN_TEST( BitSetTest );
	RUN_TEST( CollisionTest );
	RUN_TEST( DeterminismTest );
	RUN_TEST( DistanceTest );
	RUN_TEST( DynamicTreeTest );
	RUN_TEST( IdTest );
	RUN_TEST( ShapeTest );
	RUN_TEST( WorldTest );

	printf( "======================================\n" );
	printf( "All Box2D tests passed!\n" );

#if defined( _MSC_VER )
	if ( _CrtDumpMemoryLeaks() )
	{
		return 1;
	}
#endif

	return 0;
}
