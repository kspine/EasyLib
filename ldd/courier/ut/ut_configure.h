

#ifndef _LDD_UT_CONFIGURE_H_
#define _LDD_UT_CONFIGURE_H_

#include "ut_common.h"
#include "../options.h"

DECLARE_string(host_port);

using namespace ldd::courier;

#define TEST_R(t, n) void MyTest_##t##_##n() 


TEST(Configure, initialize){
    const int 		send_buffer_len = 10000;
    char				send_buffer[send_buffer_len];

    const int 		recv_buffer_len = 10000;
    char				recv_buffer[send_buffer_len];

    /************************************************************************/
    /* testing courier                                                       */
    /************************************************************************/
    InitializeZkTree(client_socket, zkStationAddr);

	int timeout=400;  const std::string root_path = "";
	Configure* configure =	NULL;
	int ret = 0;  Options option; option.timeout = timeout; 
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &configure);
	ASSERT_EQ(ret, 0);
	LOG(INFO)<<"initialize successfully";


    int bucket_size = 0;
    int replicaSize = 0;
    std::string engine_name;
    int MaxBinLogFile = 0;
    ret =  configure->GetReplicaSize(&replicaSize) ;
    ret =  configure->GetStoreEngine(&engine_name) ;
    ret =  configure->GetMaxBinLogFile(&MaxBinLogFile) ;
    ret =  configure->GetBucketSize(&bucket_size);
    EXPECT_EQ(bucket_size, atoi(BUCKET_NODE_PATH__VALUE));
    EXPECT_EQ(replicaSize, atoi(REPLICA_NODE_PATH__VALUE));
    EXPECT_EQ(MaxBinLogFile, atoi(BIN_LOG_PATH__VALUE));
    EXPECT_STREQ(engine_name.data(), ENGINE_NODE_PATH__VALUE);
    EXPECT_EQ(bucket_size, atoi(BUCKET_NODE_PATH__VALUE));

    int32_t replicaSize22 = 10;  int32_t MaxBinLogFile22 = 55555;  int32_t  bucket_size22 = 2;
    std::string engine_name22 = "gfs";
    int recv_len = 0;
    {
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(ksetOptCom);
        zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", replicaSize22);
        zkoptbody.path_len = snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, REPLICA_NODE_PATH);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }

    {
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(ksetOptCom);
        zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", MaxBinLogFile22);
        zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BIN_LOG_PATH);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }

    {
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(ksetOptCom);
        zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%d", bucket_size22);
        zkoptbody.path_len  = snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, BUCKET_NODE_PATH);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }

    {
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(ksetOptCom);
        zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "%s", engine_name22.data());
        zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s%s", BASE_CONF_PATH, ENGINE_NODE_PATH);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }

    usleep(10000);  //sleep(seconds)  // after change zk tree, should wait 10 ms for ut test

    configure->GetBucketSize(&bucket_size);
    configure->GetReplicaSize(&replicaSize) ;
    configure->GetStoreEngine(&engine_name) ;
    configure->GetMaxBinLogFile(&MaxBinLogFile) ;


    EXPECT_EQ(replicaSize, atoi(REPLICA_NODE_PATH__VALUE));
    EXPECT_EQ(MaxBinLogFile, MaxBinLogFile22);
    EXPECT_STREQ(engine_name.data(),ENGINE_NODE_PATH__VALUE);
    EXPECT_EQ(bucket_size, atoi(BUCKET_NODE_PATH__VALUE));


    //ClearZkTree(client_socket, zkStationAddr);  

    DLOG(INFO)<<"before delete";
    google::FlushLogFiles(google::INFO);
    google::FlushLogFilesUnsafe(google::INFO);

    delete configure;   // need close thread, delete reactor
    /************************************************************************/
    /*                                                                      */
    /************************************************************************/

}


const char* HOST_PORT_TEST1 = "0.0.0.0:400123";
void NodeChangeCB(int ret, NodeChangedType type, const std::string& node){
    if (type == kCreateNode){
        if(strcmp(node.data(), HOST_PORT_TEST1) != 0 ){
            fprintf(stderr, "not equal, data=%s, test1=%s\n", node.data(), HOST_PORT_TEST1);
            exit(1);
        }
        fprintf(stderr, "kCreateNode, data=%s\n", node.data());
        DLOG(INFO)<<"kCreateNode, data=%s"<<node.data();
    }else if (type == kDeleteNode){
        if(strcmp(node.data(), HOST_PORT0) == 0 ){
            fprintf(stderr, "kDeleteNode, data=%s\n", node.data());
            DLOG(INFO)<<"kDeleteNode, data=%s"<<node.data();
        }else if(strcmp(node.data(), "0.0.0.0:400123") == 0 ){
            fprintf(stderr, "kDeleteNode, data=%s\n", node.data());
            DLOG(INFO)<<"kDeleteNode, data=%s"<<node.data();
        }else{
            fprintf(stderr, "ndata=%s, HOST_PORT0=0.0.0.0:40000|123\n", node.data());
            exit(1);
        }

    }
    return ;
}


TEST(Configure, NodeListChanged){
    /************************************************************************/
    /* testing courier                                                       */
    /************************************************************************/
    const int 		send_buffer_len = 10000;
    char				send_buffer[send_buffer_len];

    const int 		recv_buffer_len = 10000;
    char				recv_buffer[send_buffer_len];
    InitializeZkTree(client_socket, zkStationAddr);

	int timeout=400;  const std::string root_path = "";
	Configure* configure =	NULL;
	int ret = 0; Options option; option.timeout = timeout;
	ret = Configure::Init(option, std::string(), FLAGS_host_port,root_path, &configure);
	ASSERT_EQ(ret, 0);
	LOG(INFO)<<"initialize successfully";
	
	std::vector<std::string> node_list;
	configure->GetNodeList(NodeChangeCB, &node_list);

    std::vector<std::string> node_list;
    configure->GetNodeList(NodeChangeCB, &node_list);

    EXPECT_EQ((int)node_list.size(), NODE_LIST_SIZE);

    int recv_len = 0;
    {	// change value of BUCKET_2 node  bucket_id == 2
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(kdeleteOptCom);
        zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s", NODE_LIST_PATH, HOST_PORT0);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }
    usleep(100000);node_list.clear();
    configure->GetNodeList(NodeChangeCB, &node_list);
    EXPECT_EQ((int)node_list.size(), NODE_LIST_SIZE-1);

    {	
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(kcreateOptCom);
        zkoptbody.value_len = snprintf(zkoptbody.value, sizeof(zkoptbody.value), "farm4");
        zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s", NODE_LIST_PATH, HOST_PORT_TEST1);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }
    usleep(100000);node_list.clear();
    configure->GetNodeList(NodeChangeCB, &node_list);
    EXPECT_EQ((int)node_list.size(), NODE_LIST_SIZE);

    {	// change value of BUCKET_2 node  bucket_id == 2
        ZkOptBody zkoptbody;
        zkoptbody.opt_type = int8_t(kdeleteOptCom);
        zkoptbody.path_len	= snprintf(zkoptbody.path, sizeof(zkoptbody.path), "%s/%s", NODE_LIST_PATH, HOST_PORT_TEST1);

        memcpy((void*)send_buffer, (void*)&zkoptbody, sizeof(ZkOptBody));
        recv_len = sendAndRecv(client_socket, &zkStationAddr,  send_buffer, sizeof(ZkOptBody),recv_buffer, recv_buffer_len);
        ASSERT_EQ(strncmp(recv_buffer, "OK!!!", strlen("OK!!!")), 0);
    }

    configure->GetNodeList(NULL, &node_list);

    usleep(100000);
    DLOG(INFO)<<"before delete";
    google::FlushLogFiles(google::INFO);
    google::FlushLogFilesUnsafe(google::INFO);

    delete configure;   // need close thread, delete reactor
    /************************************************************************/
    /*                                                                      */
    /************************************************************************/

}
#undef  TEST_R


#endif  // _LDD_UT_CONFIGURE_H_

