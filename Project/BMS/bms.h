#ifndef BMS_H
#define BMS_H

#ifdef __cplusplus
extern "C"
{
	#endif
	void setupBMS();
	void sendStartSequence();
	void sendReset();
	char *getDataString();
	char *getLoad();
	#ifdef __cplusplus
}
#endif

#endif