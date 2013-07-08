#ifndef GEN7_CROSSVMM_VMM_CPU_H
#define GEN7_CROSSVMM_VMM_CPU_H

class Cpu
{
public:
	virtual void Init() = 0;
	virtual void Execute() = 0;
};

#endif //GEN7_CROSSVMM_VMM_CPU_H

