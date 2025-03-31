`ifndef CNN_SCOREBOARD
`define CNN_SCOREBOARD

class cnn_scoreboard extends uvm_scoreboard; 
    `uvm_analysis_imp_decl(_lite)
    `uvm_analysis_imp_decl(_stream)

    `uvm_component_utils(cnn_scoreboard)

    logic[31:0] command_in_progress;  //promenljiva u koju se smesta komanda iz axi_monitora
   
    int     fd = 0;
    real    tmp;
    logic[15:0] received_image[$];    //red u koji se smestaju podaci iz master_monitora u scoreboard

    integer index;
    real    maximum = 0.0;
    real    temporary_maximum = 0.0; 

    real    converted_received_image[$]; // posle koverzije u real

    seq_item_master item_master;
    seq_item_lite   item_lite;
    uvm_analysis_imp_stream#(seq_item_master, cnn_scoreboard) port_axis; // recieve item from monitor
    uvm_analysis_imp_lite#(seq_item_lite, cnn_scoreboard)     port_axi_lite;
    
    // redovi u koje ce biti smestani "Zlatni vektori" za 1,2,3. konvoluciju
    real expected_image_1[$];           // after first  convolution
    real expected_image_2[$];           // after second convolution
    real expected_image_3[$];           // after third  convolution

    integer image_number_0 = 0;
    integer image_number_1 = 0;
    integer image_number_2 = 0;

    integer image_sent_0 = 1;
    integer image_sent_1 = 1;
    integer image_sent_2 = 1;

    // Putanje do "Zlatnih vektora" 
    string conv0_results = "../../../../../../data/conv0_output/conv0_output_10.txt";
    string conv1_results = "../../../../../../data/conv1_output/conv1_output_10.txt";
    string conv2_results = "../../../../../../data/conv2_output/conv2_output_10.txt";
       
    function new(string name = "cnn_scoreboard", uvm_component parent = null);
        super.new(name,parent);
        extract_golden_vectors();
        port_axis = new("port_axis",this);
        port_axi_lite = new("port_axi_lite",this);
    endfunction : new

    function void connect_phase(uvm_phase phase);
		super.connect_phase(phase);
	endfunction : connect_phase 

    function void write_stream(seq_item_master item);

        logic[15:0] bin_value;
        integer     binaryValue_uint;
        integer     sign;
        integer     integer_Part;
        integer     decimal_Part;
        real        realValue;
        real        stvarna_vrednost, ocekivana_vrednost;

        received_image = item.axim_s_data;

        for(int i = 0; i < received_image.size;i++) begin
            
            bin_value = received_image[i];
            binaryValue_uint = bin_value;
            sign = (binaryValue_uint>>15) & 1'b1;
            
            if(sign) begin
                binaryValue_uint = (~binaryValue_uint) + 1'b1;
            end

            integer_Part = (binaryValue_uint >> 12) & 3'b111;
            decimal_Part = binaryValue_uint & 12'hFFF;
            realValue    = $itor(integer_Part) + (($itor(decimal_Part)) /4096.0);
            
            if(sign) begin
                realValue = realValue*(-1);
            end;
            
            converted_received_image.push_back(realValue);    
        end
        
        //===================================================================
        //================ COMPARING AFTER FIRST CONVOLUTION ================
        //===================================================================
        if(converted_received_image.size() == 32768) begin
            `uvm_info(get_name(), $sformatf(" Size are equal_0"),UVM_LOW)
            
            for(int i = 0; i < 32768; i++) begin
                temporary_maximum = expected_image_1[image_number_0 + i] - converted_received_image[i];  
                if(temporary_maximum < 0)
                    temporary_maximum = temporary_maximum*(-1);
                if(temporary_maximum > maximum) begin
                    maximum = temporary_maximum;
                    index   = i;
                    stvarna_vrednost   = converted_received_image[index];
                    ocekivana_vrednost = expected_image_1[image_number_0 + index];
                    
                            
                end
            end
            `uvm_info(get_name(), $sformatf("+----------------------------------------------"),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| After first convolution"),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Image number %d",image_sent_0),UVM_LOW)                
            `uvm_info(get_name(), $sformatf("| Index       %d",index + image_number_0 + 1),UVM_LOW)// +1 jer linije u fajlovima krecu od 1
            `uvm_info(get_name(), $sformatf("| Vrednost_iz_fajla %.12f",ocekivana_vrednost),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Stvarna_vrednost  %.12f",stvarna_vrednost),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Najveca razlika   %.12f",maximum),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Najveca razlika  binarno %d",real_to_binary(maximum)),UVM_LOW) 
            `uvm_info(get_name(), $sformatf("+----------------------------------------------"),UVM_LOW)
            
            converted_received_image.delete();
            received_image.delete();
            temporary_maximum = 0;
            maximum = 0.0;
            index   = 0;
            stvarna_vrednost = 0.0;
            ocekivana_vrednost = 0.0;
            
            image_number_0 = image_number_0 + 32768; //uvecava indexe podataka koje prima iz fajla
            image_sent_0 ++; //broji koliko je slika poslato
        end
        else begin
            `uvm_info(get_name(), $sformatf("Size are not equal_0"),UVM_LOW)
        end

        //====================================================================
        //================ COMPARING AFTER SECOND CONVOLUTION ================
        //====================================================================
        
        if(converted_received_image.size() == 8192) begin
            `uvm_info(get_name(), $sformatf("Size are equal_1"),UVM_LOW)
            
            for(int i = 0; i < 8192; i++) begin
                temporary_maximum = expected_image_2[image_number_1 + i] - converted_received_image[i];  
                if(temporary_maximum < 0)
                    temporary_maximum = temporary_maximum*(-1);
                if(temporary_maximum > maximum) begin
                    maximum = temporary_maximum;
                    index   = i;
                    stvarna_vrednost   = converted_received_image[index];
                    ocekivana_vrednost = expected_image_2[image_number_1 + index];         
                end
            end
            `uvm_info(get_name(), $sformatf("+----------------------------------------------"),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| After second convolution"),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Image number %d",image_sent_1),UVM_LOW) 
            `uvm_info(get_name(), $sformatf("| Index      %d",index + image_number_1 + 1),UVM_LOW)// +1 jer linije u fajlovima krecu od 1
            `uvm_info(get_name(), $sformatf("| Vrednost_iz_fajla %.12f",ocekivana_vrednost),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Stvarna_vrednost  %.12f",stvarna_vrednost),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Najveca razlika   %.12f",maximum),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Najveca razlika  binarno %d",real_to_binary(maximum)),UVM_LOW) 
            `uvm_info(get_name(), $sformatf("+----------------------------------------------"),UVM_LOW)

            converted_received_image.delete();
            received_image.delete();
            temporary_maximum = 0;
            maximum = 0.0;
            index   = 0;
            stvarna_vrednost = 0.0;
            ocekivana_vrednost = 0.0;
            
            image_number_1 = image_number_1 + 8192; //uvecava indexe podataka koje prima iz fajla
            image_sent_1 ++; //broji koliko je slika poslato
        end
        else begin
            `uvm_info(get_name(), $sformatf("Size are not equal_1"),UVM_LOW)
        end
        
        //===================================================================
        //================ COMPARING AFTER THIRD CONVOLUTION ================
        //===================================================================

        if(converted_received_image.size() == 4096) begin
            `uvm_info(get_name(), $sformatf("Size are equal_2"),UVM_LOW)
            
            for(int i = 0; i < 4096; i++) begin
                temporary_maximum = expected_image_3[image_number_2 + i] - converted_received_image[i];  
                if(temporary_maximum < 0)
                    temporary_maximum = temporary_maximum*(-1);
                if(temporary_maximum > maximum) begin
                    maximum = temporary_maximum;
                    index   = i;
                    stvarna_vrednost   = converted_received_image[index];
                    ocekivana_vrednost = expected_image_3[image_number_2 + index];         
                end
            end
            `uvm_info(get_name(), $sformatf("+----------------------------------------------"),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| After third convolution"),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Image number %d",image_sent_2),UVM_LOW) 
            `uvm_info(get_name(), $sformatf("| Index      %d",index + image_number_2 + 1),UVM_LOW)// +1 jer linije u fajlovima krecu od 1
            `uvm_info(get_name(), $sformatf("| Vrednost_iz_fajla %.12f",ocekivana_vrednost),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Stvarna_vrednost  %.12f",stvarna_vrednost),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Najveca razlika   %.12f",maximum),UVM_LOW)
            `uvm_info(get_name(), $sformatf("| Najveca razlika  binarno %d",real_to_binary(maximum)),UVM_LOW) 
            `uvm_info(get_name(), $sformatf("+----------------------------------------------"),UVM_LOW)

            converted_received_image.delete();
            received_image.delete();
            temporary_maximum = 0;
            maximum = 0.0;
            index   = 0;
            stvarna_vrednost = 0.0;
            ocekivana_vrednost = 0.0;
            
            image_number_2 = image_number_2 + 4096; //uvecava indexe podataka koje prima iz fajla
            image_sent_2 ++; //broji koliko je slika poslato
        end
        else begin
            `uvm_info(get_name(), $sformatf("Size are not equal_2"),UVM_LOW)
        end
    endfunction : write_stream

    function void write_lite(seq_item_lite item);
        command_in_progress = item.COMMAND;
    endfunction : write_lite
    
    function void extract_golden_vectors();
        expected_image_1.delete();
        expected_image_2.delete();
        expected_image_3.delete();
        
        received_image.delete();
    
        //---------------------- Getting golden vector for first convolution ----------------------
        fd = $fopen(conv0_results,"r");
        if(fd) begin 
            `uvm_info(get_name(), $sformatf("Successfully OPENED conv0_results.txt"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                expected_image_1.push_back(tmp);
            end
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening conv0_results.txt"),UVM_HIGH)    
        end
        $fclose(fd);
        `uvm_info(get_name(), $sformatf("Successfully CLOSED conv0_results.txt\n"),UVM_HIGH)

        //---------------------- Getting golden vector for second convolution ----------------------
        fd = $fopen(conv1_results,"r");
        if(fd) begin 
            `uvm_info(get_name(), $sformatf("Successfully OPENED conv1_results.txt"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                expected_image_2.push_back(tmp);
            end
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening conv1_results.txt"),UVM_HIGH)    
        end
        $fclose(fd);
        `uvm_info(get_name(), $sformatf("Successfully CLOSED conv1_results.txt\n"),UVM_HIGH)

        //---------------------- Getting golden vector for third convolution ----------------------
        fd = $fopen(conv2_results,"r");
        if(fd) begin 
            `uvm_info(get_name(), $sformatf("Successfully OPENED conv2_results.txt"),UVM_HIGH)

            while(!$feof(fd)) begin
                $fscanf(fd ,"%f\n",tmp);
                expected_image_3.push_back(tmp);
            end
        end
        else begin
            `uvm_info(get_name(), $sformatf("Error opening conv2_results.txt"),UVM_HIGH)    
        end
        $fclose(fd);
        `uvm_info(get_name(), $sformatf("Successfully CLOSED conv2_results.txt\n"),UVM_HIGH)
    endfunction

    function logic [15:0] real_to_binary(real t);
        integer sign ;
        integer integerPart;
        integer decimalPart;
        
        logic [15:0] binaryValue;
        
        sign = (t >= 0) ? 0 : 1;
        if (sign == 0) begin
            integerPart = $rtoi(t);
            decimalPart = $rtoi((t - integerPart) * 4096);
            binaryValue = (sign << 15) | ((integerPart & 3'b111) << 12) | (decimalPart & 12'hFFF);
        end
        else begin
            integerPart = $rtoi((-1) * t);
            decimalPart = $rtoi(((-1) * t - integerPart) * 4096);
            binaryValue = (sign << 15) | (((~integerPart) & 3'b111) << 12) | ((~decimalPart) & 12'hFFF);
            binaryValue =binaryValue + 1'b1;
        end
        return binaryValue;
    endfunction

endclass : cnn_scoreboard


`endif 

/*
After first convolution
Index             19306
Vrednost_iz_fajla 0.003906250000
Stvarna_vrednost  0.001220703125
Najveca razlika   0.002685546875

After second convolution
Index             2600
Vrednost_iz_fajla 0.054199218750
Stvarna_vrednost  0.049560546875
Najveca razlika   0.004638671875

After third convolution
Index             3887
Vrednost_iz_fajla 0.062744140625
Stvarna_vrednost  0.058349609375
Najveca razlika   0.004394531250
*/
