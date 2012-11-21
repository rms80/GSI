#!/bin/bash
# [RMS] script to generate MSVC-compatible static and DLL libaries
# Assumes vcvarsall.bat has already been called (so MSVC tools like
#   lib.exe are available in path)


AtlasName=atlas
if [ $1 ] ; then
    AtlasName=atlas_${1}
fi
echo Building Windows Libraries named ${AtlasName}


defname=${AtlasName}.def
dllname=${AtlasName}.dll
msvclibname=${AtlasName}.lib
expname=${AtlasName}.exp
staticname=${AtlasName}_static.lib
staticblasname=${AtlasName}_cblas_static.lib


# create the .def file used for linking Atlas .dlls
# uncomment or comment the various routines below as needed to get what you
# want in the DLL


# C BLAS interface
Cexports=(\
#single precision routines
	cblas_sdsdot cblas_sdot cblas_snrm2 cblas_sasum cblas_scnrm2 \
	cblas_scasum cblas_sscal cblas_isamax \
	cblas_sswap cblas_scopy cblas_saxpy catlas_saxpby catlas_sset \
	cblas_srotg cblas_srotmg cblas_srot cblas_srotm \
	cblas_sgemv cblas_sgbmv cblas_strmv cblas_stbmv cblas_stpmv \
	cblas_strsv cblas_stbsv cblas_stpsv \
	cblas_ssymv	cblas_ssbmv cblas_sspmv cblas_sger cblas_ssyr \
	cblas_sspr cblas_ssyr2 cblas_sspr2 \
	cblas_sgemm cblas_ssymm cblas_ssyrk cblas_ssyr2k cblas_strmm cblas_strsm \
#double precision routines
	cblas_dsdot cblas_ddot cblas_dnrm2 cblas_dasum cblas_dznrm2 \
	cblas_dzasum cblas_dscal cblas_idamax \
	cblas_dswap cblas_dcopy cblas_daxpy catlas_daxpby catlas_dset \
	cblas_drotg cblas_drotmg cblas_drot cblas_drotm \
	cblas_dgemv cblas_dgbmv cblas_dtrmv cblas_dtbmv cblas_dtpmv \
	cblas_dtrsv cblas_dtbsv cblas_dtpsv \
	cblas_dsymv cblas_dsbmv cblas_dspmv cblas_dger cblas_dsyr \
	cblas_dspr cblas_dsyr2 cblas_dspr2 \
	cblas_dgemm cblas_dsymm cblas_dsyrk cblas_dsyr2k cblas_dtrmm cblas_dtrsm \
#single precision complex routines
#	cblas_cdotu_sub cblas_cdotc_sub cblas_cscal cblas_csscal cblas_icamax \
#	cblas_cswap cblas_ccopy cblas_caxpy catlas_caxpby catlas_cset \
#	cblas_crotg cblas_csrot \
#	cblas_cgemv cblas_cgbmv	cblas_ctrmv cblas_ctbmv cblas_ctpmv \
#	cblas_ctrsv cblas_ctbsv cblas_ctpsv \
#	cblas_chemv cblas_chbmv cblas_chpmv cblas_cgeru cblas_cgerc \
#	cblas_cher cblas_chpr cblas_cher2 cblas_chpr2 \
#	cblas_cgemm cblas_csymm cblas_csyrk cblas_csyr2k cblas_ctrmm cblas_ctrsm \
#	cblas_chemm cblas_cherk cblas_cher2k \
#double precision complex routines
#	cblas_zdotu_sub cblas_zdotc_sub cblas_zscal cblas_zdscal cblas_izamax \
#	cblas_zswap cblas_zcopy cblas_zaxpy catlas_zaxpby catlas_zset \
#	cblas_zrotg cblas_zdrot \
#	cblas_zgemv cblas_zgbmv cblas_ztrmv cblas_ztbmv cblas_ztpmv	\
#	cblas_ztrsv cblas_ztbsv cblas_ztpsv \
#	cblas_zhemv cblas_zhbmv cblas_zhpmv cblas_zgeru cblas_zgerc \
#	cblas_zher cblas_zhpr cblas_zher2 cblas_zhpr2 \
#	cblas_zgemm cblas_zsymm cblas_zsyrk cblas_zsyr2k cblas_ztrmm cblas_ztrsm \
#	cblas_zhemm cblas_zherk cblas_zher2k \
#other stuff
#	cblas_errprn \
# CLAPACK interface subset
	clapack_sgesv clapack_sgetrf clapack_sgetrs clapack_sgetri \
	clapack_sposv clapack_spotrf clapack_spotrs clapack_spotri \
	clapack_slauum clapack_strtri \
	clapack_dgesv clapack_dgetrf clapack_dgetrs clapack_dgetri \
	clapack_dposv clapack_dpotrf clapack_dpotrs clapack_dpotri \
	clapack_dlauum clapack_dtrtri \
	clapack_cgesv clapack_cgetrf clapack_cgetrs clapack_cgetri \
	clapack_cposv clapack_cpotrf clapack_cpotrs clapack_cpotri \
	clapack_clauum clapack_ctrtri \
	clapack_zgesv clapack_zgetrf clapack_zgetrs clapack_zgetri \
	clapack_zposv clapack_zpotrf clapack_zpotrs clapack_zpotri \
	clapack_zlauum clapack_ztrtri)
#Fortran BLAS interface
#FortranExports=(\
#single precision routines
#	saxpy scasum scnrm2 scopy sdot sdsdot sgbmv sgemm \
#	sgemv sger snrm2 srot srotg srotm srotmg ssbmv sscal \
#	sspmv sspr sspr2 sswap ssymm ssymv ssyr ssyr2 ssyr2k \
#	ssyrk stbmv stbsv stpmv stpsv strmm strmv strsm strsv \
#double precision routines
#	dasum daxpy dcabs1 dcopy ddot dgbmv dgemm dgemv \
#	dger dnrm2 drot drotg drotmg dsbmv dscal dsdot \
#	dspmv dspr dspr2 dswap dsymm dsymv dsyr dsyr2 dsyr2k \
#	dsyrk dtbmv dtbsv dtpmv dtpsv dtrmm dtrmv dtrsm dtrsv \
#	dzasum dznrm2 icamax idamax isamax izamax lsame sasum \
#single precision complex routines
#	caxpy ccopy cdotc cdotu cgbmv cgemm cgerc cgeru chbmv \
#	chemm chemv cher cher2 cher2k cherk chpmv chpr chpr2 \
#	crotg cscal csrot csscal cswap csymm csyr2k csyrk \
#	ctbmv ctbsv ctpmv ctpsv ctrmm ctrmv ctrsm ctrsv \
#double precision complex routines
#	zaxpy zcopy zdotc zdotu zdrot zdscal zgbmv zgemm \
#	zgemv zgerc zgeru zhbmv zhemm zhemv zher zher2 zher2k \
#	zherk zhpmv zhpr zhpr2 zrotg zscal zswap zsymm \
#	zsyr2k zsyrk ztbmv ztbsv ztpmv ztpsv ztrmm ztrmv ztrsm ztrsv \
#other stuff
#	xerbla \
# LAPACK interface subset
#	sgesv sgetrf sgetrs sgetri sposv spotrf spotrs spotri \
#	slauum strtri \
#	dgesv dgetrf dgetrs dgetri dposv dpotrf dpotrs dpotri \
#	dlauum dtrtri \
#	cgesv cgetrf cgetrs cgetri cposv cpotrf cpotrs cpotri \
#	clauum ctrtri \
#	zgesv zgetrf zgetrs zgetri zposv zpotrf zpotrs zpotri \
#	zlauum ztrtri)


#create exports file
echo "EXPORTS" > ${defname}
#write C exports (these are straight C, so do not need any name mangling or und{erscores)
for (( i = 0 ; i < ${#Cexports[*]} ; i++ )) ; do
	export=${Cexports[$i]}
	echo -e "${export}=${export}" >> ${defname}
done
#write Fortran exports (have to append trailing underscore)
#for (( i = 0 ; i < ${#FortranExports[*]} ; i++ )) ; do
#	export=${FortranExports[$i]}
#	echo -e "${export}=${export}_" >> ${defname}
#done


# libraries and compiler (system-dependent...)
GCC=gcc
CLIBPATH=/usr/lib/mingw
CLIBPATH2=/lib/gcc/i686-pc-mingw32/3.4.4/
mingwclib="$CLIBPATH2/libg2c.a $CLIBPATH/libmoldname.a"

# generate DLL from def file
# This generates a library containing all functions exported in the .def and
# any functions which they call.
echo 'Linking DLL'
${GCC} -mno-cygwin -shared -o ${dllname} ${defname} \
    liblapack.a libcblas.a libf77blas.a libatlas.a \
    -Wl,--enable-auto-import \
    -Wl,--no-whole-archive ${mingwclib}

# Create import library for use with MS Visual C++
echo 'creating MSVC-compatible dll import libary'
lib /DEF:${defname} /MACHINE:IX86 /OUT:${msvclibname}

# generate static library with everything in it
echo 'Generating static atlas library'
lib liblapack.a libcblas.a libf77blas.a libatlas.a /OUT:${staticname}

# generate cblas-only static library
lib libcblas.a libf77blas.a libatlas.a /OUT:${staticblasname}



# install files
cp ${dllname} ../../../lib
cp ${defname} ../../../lib
cp ${expname} ../../../lib
cp ${msvclibname} ../../../lib
cp ${staticname} ../../../lib
cp ${staticblasname} ../../../lib

cp ../../include/cblas.h ../../../include/atlas_cblas.h
cp ../../include/clapack.h ../../../include/atlas_clapack.h