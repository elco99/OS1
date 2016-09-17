
    #include <iostream>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <stdlib.h>
    #include <unistd.h>
	#include <ctime>
    #include <ctype.h>
	#include <fstream>
	#include <sstream>
	#include <vector>	
     
    using namespace std;

    int portNum = 1800;
    bool isExit = false;
    int bufsize = 256 ;
    //creacion del la struct del thread_pool
 	struct pthread_item{
	    pthread_t thread;
	    bool state;
	};
   	pthread_item * thread_pool = new pthread_item[10];
   	//struct para la rutina con el numero de socket y el numero de thread, el cual servira para cambiar su estado una vez el hilo termine su ejecucion.
   	typedef struct thread_params{
   		int socket_number;
   		int thread_number;
   	}thread_data;
    void * rutinaLectura(void *);
    int main()
    {        
       

    	int socket_listen, socket_connection;
        struct sockaddr_in server_addr;
        socklen_t size;
        pthread_t hilo;
     
        /* ---------- ESTABLISHING SOCKET CONNECTION ----------*/
        /* --------------- socket() function ------------------*/
     
     
        cout << "\n=> Socket server has been created on the port " ;
     	
       
        socket_connection = socket(AF_INET, SOCK_STREAM, 0);
     
     
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
        server_addr.sin_port = htons(portNum);
     
        /* ---------- BINDING THE SOCKET ---------- */
        /* ---------------- bind() ---------------- */
        socket_listen = socket(AF_INET, SOCK_STREAM , 0);
     	int error = bind(socket_listen, (struct sockaddr*)&server_addr,sizeof(server_addr));
     	if (error < 0)
        {

    		portNum = 1600;

	        socket_connection = socket(AF_INET, SOCK_STREAM, 0);
	     
	     
	        server_addr.sin_family = AF_INET;
	        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    		server_addr.sin_port = htons(portNum);

     		error = bind(socket_listen, (struct sockaddr*)&server_addr,sizeof(server_addr));
     		if(error < 0){
	            portNum = 1700;

		        socket_connection = socket(AF_INET, SOCK_STREAM, 0);
		     
		     
		        server_addr.sin_family = AF_INET;
		        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	    		server_addr.sin_port = htons(portNum);

	     		error = bind(socket_listen, (struct sockaddr*)&server_addr,sizeof(server_addr));
	     		if(error < 0){
		            cout << "=> Error binding connection, the socket has already been established..." << endl;
		            return -1;

	     		}

     		}
        }
        cout<< portNum << endl;
     	listen(socket_listen, SOMAXCONN);
     	size = sizeof(server_addr);
     	
		for (int i = 0; i < 10; ++i)
		{
			thread_pool[i].state = false;
		}


     	for (;;)
     	{
     		if(socket_connection = accept(socket_listen, (struct sockaddr *)&server_addr,&size ) ){
     			//pthread_create(&hilo,NULL, rutinaLectura,(void *)socket_connection);
     			//pthread_join(hilo,NULL);

     			for (int i = 0; i < 10; ++i){
     				if(!thread_pool[i].state){
     					thread_pool[i].state = true;
     					thread_params params;
     					params.socket_number = socket_connection;
     					params.thread_number = i;
     					pthread_create(&thread_pool[i].thread,NULL, rutinaLectura,(void *)&params);
     					break;
     				}
     			}

     			
     			//if(buffer_str.find("get") == string::npos || buffer_str.find("GET") == string::npos){
     			//	cout << "si ta";
     			//}

     		}
     	}

        return 0;
    }

void * rutinaLectura(void * params){
	//casteo de la struct
	thread_params * t_data = (thread_params *)params;
	int sock = t_data->socket_number;//t_data.socket_number;
	int thread_id = t_data->thread_number;
	//thread_params t_data = (thread_data ) ;
	/*cout << "========================================================="<<endl;
	cout << "tdata socket "<<t_data->socket_number << endl;
	cout << "tdata thread " << t_data->thread_number << endl;
	cout << "state  	" <<thread_pool[t_data->thread_number].state << " * "<< endl;*/
	stringstream fullbuffer;
	char read_data[bufsize];
	ssize_t read_bytes;
	vector <unsigned char> bufferType;


	do{
		read_bytes = read(sock, read_data, bufsize);
		if(read_bytes > 0){     					
			string buffer_str(read_data);

			for (int i = 0; i < read_bytes; ++i)
			{
				//cout << read_data[i];
				fullbuffer << read_data[i];
				bufferType.push_back(read_data[i]);
			}
		}
		//cout << read_bytes << " :: " << bufsize<<endl;
	}while(read_bytes == bufsize);
	//cout << fullbuffer.str() << endl;
	//string buffer_str(buffer);
	string ss = fullbuffer.str();
	char  *buffer = new char[ss.size()];
	strcpy(buffer, ss.c_str());
	//cout << buffer <<endl;

	

	int len;
	int pivot;
	string page;

	string buff(buffer);
	char * contains_get = strstr(buffer, "GET");
	char * contains_put = strstr(buffer, "PUT");
	char * contains_post = strstr(buffer, "POST");
     			
	if(contains_get != NULL){	
		pivot = buff.find_first_of('/');
		if (pivot != string::npos)
		{
			page = buff.substr(pivot,buff.length());
			pivot = page.find_first_of(" ");
			if (pivot != string::npos)
			{
				string url= page.substr(0,pivot);
				const char * url2 = url.c_str();
				string content;
				char buffer2 [1024];

				//FILE *pFile = fopen(string("./mi_web").append(url).c_str(),"rb");
				vector< char > fileContents;

				//cout << url << endl;
				//cout << fullbuffer.str() << endl;
				ifstream file(string("./mi_web").append(url).c_str(), ios::in | ios::binary | ios::ate);
				//ifstream f(string("./mi_web").append(url).c_str(), ios::in| ios::binary |ios::ate);
				int content_buf;
				if (file  != 0)
				{
					stringstream content_length,content_type , date;

					for (int i = 0; i < url.size(); ++i)
					{
						if(url[i] == '.'){
							for (int j = i+1; j< url.size(); ++j)
							{
								content_type << url[j];
							}
							break;
						}
					}


      			
			      	char buf_time[40];
					time_t now = time(0);
					struct tm tm = *gmtime(&now);
			  		strftime(buf_time, sizeof buf_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
			  		
					//char * str2 = buf;
					//time_t time = time(0);
 					//strcpy (str1,str2);
					date << "Date: " << buf_time;
					content.append("HTTP/1.1 200 OK\r\n");
					content.append(date.str());
					content.append("\r\n");
					content.append("Server: cualquiercosaaqui\r\n");					
					content_length<< "Content-Length: " << file.tellg();
					content.append(content_length.str());
					content.append("\r\n");

					if(content_type.str() == "html")						
							content.append("Content-type: text/html\r\n");
					else if (content_type.str() == "txt")						
							content.append("Content-type: text/plain\r\n");
					else if (content_type.str() == "png")						
							content.append("Content-type: image/png\r\n");
					else if (content_type.str() == "jpeg")						
							content.append("Content-type: image/jpeg\r\n");
					else if (content_type.str() == "pdf")	
							content.append("Content-type: appllication/pdf\r\n");
					else if (content_type.str() == "mp3")	
							content.append("Content-type: audio/mp3\r\n");
					else if (content_type.str() == "docx")	
							content.append("Content-type: appllication/msword\r\n");
					else if (content_type.str() == "doc")	
							content.append("Content-type: appllication/msword\r\n");
					else if (content_type.str() == "gif")	
							content.append("Content-type: image/gif\r\n");
					else if (content_type.str() == "css")	
							content.append("Content-type: text/css\r\n");
					else if (content_type.str() == "jpe")	
							content.append("Content-type: image/jpeg\r\n");
					else if (content_type.str() == "jpg")	
							content.append("Content-type: image/jpeg\r\n");

					content.append("Connection: Closed\r\n\r\n");
				
					content_buf += content.size();

				  	fileContents.resize(file.tellg());
					file.seekg(0, ios::beg);
					file.read(&fileContents[ 0 ], fileContents.size());
					
					//cout << "entro aqui! " << endl;
					stringstream ss;
					for( unsigned i = 0; i != file.gcount(); ++i )
					{
					  ss << fileContents[ i ];
					}
					content.append(ss.str());
					content_buf += file.gcount();
					
				}else{
					stringstream content_length,content_type , date;
					char buf_time[40];
					time_t now = time(0);
					struct tm tm = *gmtime(&now);
			  		strftime(buf_time, sizeof buf_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
			  		
					//char * str2 = buf;
					//time_t time = time(0);
 					//strcpy (str1,str2);
					date << "Date: " << buf_time;
					content.append("HTTP/1.1 404 File not found\r\n");
					content.append(date.str());
					content.append("\r\n");
					content.append("Server: cualquiercosaaqui\r\n");					
					content_length<< "Content-Length: " << file.tellg();
					content.append(content_length.str());
					content.append("\r\n");
					content.append("Connection: Closed\r\n");		
					content.append("Content-type: text/html\r\n\r\n");
					
					send(sock,content.c_str(),content.length(),0);
				}


				//cout << content << endl;
				delete[] buffer;
				send(sock, content.c_str(), content_buf, 0);	
				close(sock);
			}
		}

	}else if(contains_put != NULL){   // CONSTAINS PUT
        pivot = buff.find_first_of("?");
        if (pivot != string::npos)
        {
            page = buff.substr(pivot,buff.length());
            pivot = page.find_first_of(" ");
            if (pivot != string::npos)
            {
                string url= page.substr(1,pivot-1);
                const char * url2 = url.c_str(); //index.txt

               
                //cout <<endl<<"----------------------------------------------------"<<endl;

               	/*cout <<"File file_description str: "<< file_description.str() << " + " << endl;
               	cout << "file description size : " << file_description.str().size()<<endl;
                */
               	//cout << "fullbuffer : " << fullbuffer.str() << endl;
               	//int content_length_pivot = page.substr(, page.find_first_of(" HTTP") - page.find_first_of("=") );
               	stringstream content_string;
               	page = buff.substr(0,buff.length());
               //	cout << fullbuffer.str() << endl;
               	for (int i = 0; i < 300; ++i)
               	{
               		stringstream temp;
               		for (int j = i; j < i+16; ++j)
               		{
               			temp << fullbuffer.str()[j];
               		}
               		//temp = fullbuffer.str().substr(i, i+16);
               		if(temp.str() == "Content-Length: "){
               			for (int j = i+16; j < 300-i; ++j)
               			{
               				if(fullbuffer.str()[j] == '\n')
               					break;
           					content_string << fullbuffer.str()[j];
               				
               			}
               			
               		}
               	}
               	/*cout << url << endl;
              	//cout << fullbuffer.str() << endl;
               	cout <<endl<< endl;
               	cout << "content length" << content_string.str()<<endl;
               	cout << "size buffer"<< fullbuffer.str().length()<<endl;
				//cout << fullbuffer.str()<<endl;*/
               	int data_start_point = fullbuffer.str().length() -atoi(content_string.str().c_str()), 
               		data_end_point  = fullbuffer.str().length();
               	//cout << "Data start point " << data_start_point << endl;
               	if(data_start_point > 0){

	         			//cout << fullbuffer.str()[data_start_point]<< "----" << endl;
						stringstream temp;
							//string temp = file_description.str();
						/*cout << "=========first data ==================" << endl;		
						for (int i = data_start_point; i < data_start_point+10; ++i)
						{
							cout << fullbuffer.str()[i];
						}
						cout <<endl<<"============end =================" << endl;
						*/
	               		const char * file_data = fullbuffer.str().substr(data_start_point, data_end_point).c_str();
	               		vector <unsigned char> to_be_sent;
	               		for (int i = data_start_point; i < data_end_point ; ++i)
	               		{
	               			to_be_sent.push_back(bufferType[i]);
	               		}
	               		int bytes_tobe_written = data_end_point - data_start_point;
	               		//cout << "bytes to be written:" << bytes_tobe_written  <<endl;
	               		for (int i =0; i < bytes_tobe_written; ++i)
		               	{
		               		temp << file_data[i];
		               		//cout << file_data[i];
		               	}

						/*cout <<"length de temp (enviado a escritura) : " <<temp.str().length() << endl;
		               
		               	cout << endl;*/

		                ofstream pFile(string("./mi_web/").append(url).c_str(), ios::out | ios::trunc | ios::binary); 
		                if (pFile )
		                {		                   
		                    pFile.write((const char*)&to_be_sent[0], bytes_tobe_written);
		                    //pFile.write(eof.c_str(), 1);
		                    //cout << file_description.str().size()<<endl;
		                    pFile.close(); 
		                }

	                	string content;
	                   	stringstream content_length,content_type , date;
						char buf_time[40];
						time_t now = time(0);
						struct tm tm = *gmtime(&now);
				  		strftime(buf_time, sizeof buf_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
				  		
						//char * str2 = buf;
						//time_t time = time(0);
	 					//strcpy (str1,str2);
						date << "Date: " << buf_time;
						content.append("HTTP/1.1 200 OK\r\n");
						content.append(date.str());
						content.append("\r\n");
						content.append("Server: cualquiercosaaqui\r\n");					
						content.append("Content-Length: 0\r\n"); 
						content.append("Connection: Closed\r\n");		
						content.append("Content-type: text/html\r\n\r\n");
						send(sock, content.c_str(), content.size(), 0);
					
				}else{
					string content;
					stringstream content_length,content_type , date;
					char buf_time[40];
					time_t now = time(0);
					struct tm tm = *gmtime(&now);
			  		strftime(buf_time, sizeof buf_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
			  		
					//char * str2 = buf;
					//time_t time = time(0);
 					//strcpy (str1,str2);
					date << "Date: " << buf_time;
					content.append("HTTP/1.1 400 Bad Request\r\n");
					content.append(date.str());
					content.append("\r\n");
					content.append("Server: cualquiercosaaqui\r\n");					
					content.append("Content-Length: 0\r\n");
					content.append("Connection: Closed\r\n");		
					content.append("Content-type: text/html\r\n\r\n");
					
					send(sock,content.c_str(),content.length(),0);
				}
                
				delete[] buffer;
       		    //send(sock, content.c_str(), content.size(), 0);
                close(sock);
            }
        }
        //END PUT
    }
    else if(contains_post != NULL){

    	pivot = buff.find_first_of("?");
        if (pivot != string::npos)
        {
            page = buff.substr(pivot,buff.length());
            pivot = page.find_first_of(" ");
            if (pivot != string::npos)
            {
                string url= page.substr(1,pivot-1);
                const char * url2 = url.c_str(); //index.txt
                ifstream inFile(string("./mi_web/").append("table_format.html").c_str(), ios::out | ios::binary | ios::ate);
                if (inFile != 0)
                {
                	//cout <<endl<<"----------------------------------------------------"<<endl;
	                stringstream value, key, table_stream;

	                bool flag = false;

	                //lectura de la plantilla

					vector< char > fileContents;
	                fileContents.resize(inFile.tellg());
	                inFile.seekg(0, ios::beg);
					inFile.read(&fileContents[0], fileContents.size() );
					stringstream table_data;
					for( unsigned i = 0; i != inFile.gcount(); ++i )
					{
					  table_data << fileContents[ i ];
					}
					//
					//cout << table_data.str() << endl<<"========"<<endl;
					stringstream first_half_table_data;
					stringstream second_half_table_data;
					bool first_half_flag = false;
					for (int i = 0; i < fileContents.size(); ++i)
	               	{
	               		if(!first_half_flag){               			
	               			stringstream temp;
		               		for (int j = i-7; j < i; ++j)
		               		{
		               			if (i >= 7)
		               			{
		               				temp << table_data.str()[j];
		               			}
		               			
		               		}	
		               		if(temp.str() == "<table>" && !first_half_flag)
		               			first_half_flag = true;
		               		
	               		}
	               		//temp = fullbuffer.str().substr(i, i+16);
	               		
	               		if(!first_half_flag)
	               			first_half_table_data << fileContents[i];
	               		else
	               			second_half_table_data << fileContents[i];
	               	}
	              // 	cout << first_half_table_data.str() << endl << "-----------------------" << endl;
	               	//cout << second_half_table_data.str() << endl <<endl;

	               	//separamos los distintos keys y values y los anexamos a first_half_table_data
					//cout << "url " << url << endl << "url length " << url.length() << endl;
	                for (int i = 0; i < url.length(); ++i)
	                {
	                	if (url[i] == '=' && i != url.length() -1)
	                	{
	                		flag = true;
	                		//cout << "encontro =" << endl;
	                	}else{
	                		if (flag || i == url.length() -1)
	                		{
	                			if (url[i] == '&' || i == url.length() -1)
	                			{
	                				flag = false;
	                				//AQUI YA TENEMOS KEY Y VALUE AHORA TOCA ESCRIBIRLOS Y REINICIARLOS
	                				first_half_table_data << "\n<tr>\n";
	                				first_half_table_data << "<td>" << key.str() << "<td>\n";
	                				first_half_table_data << "<td>" << value.str() << "<td>\n";
	                				first_half_table_data << "<tr>\n";
	                				/*
	                				first_half_table_data << "\n\t\t\t<tr>\n";
	                				first_half_table_data << "\t\t\t\t<td>" << key.str() << "<td>\n";
	                				first_half_table_data << "\t\t\t\t<td>" << value.str() << "<td>\n";
	                				first_half_table_data << "\t\t\t<tr>\n";
									
	                				*/
									
									//cout << "row : " << key.str() << " : " << value.str() << endl;
	                				//cout << i << endl;
	                				key.str("");
	                				key.clear();
	                				value.str("");
	                				value.clear();


	                			}else{
	                				value << url[i];
	                			}
	                		}else{

	                			key << url[i];
	                		}
	                	}
	                }

	                //creamos el header para el response data

	            	char buf_time[40];
					time_t now = time(0);
					struct tm tm = *gmtime(&now);
			  		strftime(buf_time, sizeof buf_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
			  		
					//char * str2 = buf;
					//time_t time = time(0);
						//strcpy (str1,str2);
			  		string content;
			  		int content_size = first_half_table_data.str().length() + second_half_table_data.str().length();
			  		stringstream content_size_str,date;
			  		content_size_str << content_size;
					date << "Date: " << buf_time;
					content.append("HTTP/1.1 200 OK\r\n");
					content.append(date.str());
					content.append("\r\n");
					content.append("Server: cualquiercosaaqui\r\n");	
					content.append("Content-Length: " );
					content.append(content_size_str.str());
					content.append("\r\n");						
					content.append("Content-type: text/html\r\n");
					content.append("Connection: Closed\r\n\r\n");
	                stringstream full_table;
	               // cout << first_half_table_data.str() <<endl;
	                full_table << first_half_table_data.str();
	                full_table << second_half_table_data.str();

	               	//cout << full_table.str() <<endl;
	               	content.append(full_table.str());
	               	//cout <<"File file_description str: "<< file_description.str() << " + " << endl;
	               	//cout << "file description size : " << file_description.str().size()<<endl;
	                
	               	//cout << "fullbuffer : " << fullbuffer.str() << endl;
	               	//int content_length_pivot = page.substr(, page.find_first_of(" HTTP") - page.find_first_of("=") );
	               	
					send(sock, content.c_str(), content.length(), 0);	
	                close(sock);	
                }else{
                	string content;
					stringstream content_length,content_type , date;
					char buf_time[40];
					time_t now = time(0);
					struct tm tm = *gmtime(&now);
			  		strftime(buf_time, sizeof buf_time, "%a, %d %b %Y %H:%M:%S %Z", &tm);
			  		
					//char * str2 = buf;
					//time_t time = time(0);
 					//strcpy (str1,str2);
					date << "Date: " << buf_time;
					content.append("HTTP/1.1 500 Internal Server Error\r\n");
					content.append(date.str());
					content.append("\r\n");
					content.append("Server: cualquiercosaaqui\r\n");					
					content.append("Content-Length: 0\r\n");
					content.append("Connection: Closed\r\n");		
					content.append("Content-type: text/html\r\n\r\n");
					
					send(sock,content.c_str(),content.length(),0);
                }
                
            }
        }//fin de POST
    }//end de POST
    sleep(3)  ;
    cout << "state b4 refreshing" << thread_pool[thread_id].state<<endl; 
    cout << "thread id" << thread_id<<endl;
	thread_pool[thread_id].state = false;
	cout << "state after refreshingn" << thread_pool[thread_id].state<< endl;
    cout << "thread id" << thread_id<<endl;
}




