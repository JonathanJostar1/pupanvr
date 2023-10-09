#include "TJsonRPCServerProcess.h"
#include "jsonrpc-c.h"
#include "tlog.h"

TJsonRPCServerProcess* TJsonRPCServerProcess::m_instance = NULL;


cJSON * say_hello(jrpc_context * ctx, cJSON * params, cJSON *id) 
{
    LOG(INFO) << "params:" << cJSON_Print(params) << ", id:" << cJSON_Print(id) << endl;
    LOG(INFO) << "get sat hello!" << endl;
	return cJSON_CreateString("Hello!");
}

#if 0
cJSON * add(jrpc_context * ctx, cJSON * params, cJSON *id) {
	cJSON * a = cJSON_GetArrayItem(params,0);
	cJSON * b = cJSON_GetArrayItem(params,1);
	return cJSON_CreateNumber(a->valueint + b->valueint);
}

cJSON * exit_server(jrpc_context * ctx, cJSON * params, cJSON *id) {
	jrpc_server_stop(&my_server);
	return cJSON_CreateString("Bye!");
}
#endif

TJsonRPCServerProcess* TJsonRPCServerProcess::getInstance()
{
    if(!m_instance)
    {
        m_instance = new TJsonRPCServerProcess();     
    }

    return m_instance;
}

TJsonRPCServerProcess::TJsonRPCServerProcess()
{
    m_server_port = 55001;
}

TJsonRPCServerProcess::~TJsonRPCServerProcess()
{

}

void TJsonRPCServerProcess::process()
{
    int ret = 0;
    struct jrpc_server jrpcServer;
    
    ret = jrpc_server_init(&jrpcServer, m_server_port);
    if(ret != 0)
    {
        LOG(ERROR) << "jrpc_server_init failure! at port:" << m_server_port << endl;
        return;
    }

    jrpc_register_procedure(&jrpcServer, say_hello, "sayHello", NULL);
    jrpc_server_run(&jrpcServer);
	jrpc_server_destroy(&jrpcServer);
}

bool TJsonRPCServerProcess::setJsonRpcServerPort(int port)
{
    if(this->status())
    {
        LOG(ERROR) << "setJsonRpcServerPort failuer! thread is runing!" << endl;
        return false;
    }   

    m_server_port = port;
    return true;
}
