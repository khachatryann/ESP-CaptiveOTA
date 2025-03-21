idf_component_register(
    SRCS "main.c" 
    	 "../ota/ota_handler.c"
    	 "../webserver/webserver.c"
    	 "../wifi/wifi_manager.c"  
    	 
    INCLUDE_DIRS "."
    			 "../ota"
    			 "../webserver"
    			 "../wifi"
    
    EMBED_FILES "root.html"
)
                       