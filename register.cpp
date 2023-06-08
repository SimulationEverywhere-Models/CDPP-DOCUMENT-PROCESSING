
#include "modeladm.h" 
#include "mainsimu.h"

#include "scan.h"
#include "interpret.h"
#include "verify.h"
#include "queueproc.h"
#include "generatproc.h"


void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Scan>() , "Scan" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Interpret>() , "Interpret" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Verify>() , "Verify" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Queueproc>() , "Queueproc" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Generatorproc>() , "Generatorproc" ) ;

}
