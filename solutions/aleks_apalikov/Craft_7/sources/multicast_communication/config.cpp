#include "config.h"


config::~config(void)
{
	conf.close();
}
