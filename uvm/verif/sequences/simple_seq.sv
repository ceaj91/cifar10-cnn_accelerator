`ifndef SIMPLE_SEQ_SV
`define SIMPLE_SEQ_SV

class cnn_simple_seq extends base_seq;

    `uvm_object_utils(cnn_simple_seq)

    cnn_seq_item cnn_item;
    
    // dodao sam ove promenljive da lakse kontrolisemo koja konvolucija ce se izvrsavati
    int do_conv_1   = 1;
    int do_conv_2   = 1;
    int do_conv_3   = 1;

    function new(string name = "cnn_simple_seq");
      super.new(name);
      
    endfunction

    virtual task body();   
        
        extract_data();
        cnn_item = cnn_seq_item::type_id::create("cnn_item");

                //----------- RESET IP ------------------
                `uvm_info(get_name(), $sformatf("RESET IP"),   UVM_HIGH)  
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00010000000000;
                    cnn_item.axim_s_ready   = 1'b0;
                    //cnn_item.axis_s_valid   = 1'b1;
                    //cnn_item.axis_s_data_in = bias;
                    //cnn_item.axis_s_last    = 1'b1;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                finish_item(cnn_item);
                
                // ----------- 1. SENDING BIAS_0 -----------
                `uvm_info(get_name(), $sformatf("1. SENDING BIAS"),   UVM_HIGH)  
                for(int i = 0 ; i < 128; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000001;
                    cnn_item.axis_s_valid   = 1'b1; // moguc nacin randomizacije "cnn_item.axis_s_valid.randomize()"" promenio sam constsraint 90% i 10% 
                    cnn_item.axis_s_data_in = real_to_binary(bias[i]);
                    if(i == 127)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
            
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
            
        if(do_conv_1) begin 
        
                // ----------- 2. SENDING WEIGHTS_0-----------
                `uvm_info(get_name(), $sformatf("2. SENDING WEIGHTS_0"),   UVM_HIGH)  
                for(int i = 0 ; i < 864; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000010;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights0[i]);
                    if(i == 863)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
            
                // ----------- 3. SENDING IMAGE_0 -----------
                `uvm_info(get_name(), $sformatf("3. SENDING IMAGE_0"),   UVM_HIGH)
                for(int i = 0 ; i < 3468; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000100;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(image0[i]);
                    if(i == 3467)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 4. START FIRST CONVOLUTION LAYER -----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("4. START FIRST CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00000000001000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 5. READING IMAGE AFTER FIRST CONVOLUTION LAYER -----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("5. READING IMAGE AFTER FIRST CONVOLUTION LAYER"),   UVM_HIGH)
                    cnn_item.input_command  = 14'b00100000000000;
                    //cnn_item.axis_s_valid   = 1'b0;
                    //cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b1;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
        end

        if(do_conv_2) begin
                
                //----------- RESET IP ------------------
                `uvm_info(get_name(), $sformatf("RESET IP"),   UVM_HIGH)  
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00010000000000;
                    //cnn_item.axis_s_valid   = 1'b1;
                    //cnn_item.axis_s_data_in = bias;
                    //cnn_item.axis_s_last    = 1'b1;
                    //cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                // OVO MOZE DA BUDE I DRUGI TEST
        
                // ----------- 6. SENDING HALF WEIGHTS_1 -----------
                `uvm_info(get_name(), $sformatf("6. SENDING HALF WEIGHTS_1"),   UVM_HIGH)  
                for(int i = 0 ; i < 4608; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000010000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights1[i]);
                    if(i == 4607)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;  
                finish_item(cnn_item);

                // ----------- 7. SENDING IMAGE_2 -----------
        `       uvm_info(get_name(), $sformatf("7. SENDING IMAGE_2"),   UVM_HIGH)
                for(int i = 0 ; i < 10368; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000100000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(image1[i]);
                    if(i == 10367)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 8. START SECOND CONVOLUTION LAYER-----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("8. START SECOND CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00000001000000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 9. SENDING HALF WEIGHTS_1 -----------
                `uvm_info(get_name(), $sformatf("9. SENDING HALF WEIGHTS_1"),   UVM_HIGH)  
                for(int i = 0 ; i < 4608; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000010000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights1[4608+i]);
                    if(i == 4607)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 10. START SECOND CONVOLUTION LAYER-----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("10. START SECOND CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00000001000000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 11. READING IMAGE AFTER SECOND CONVOLUTION LAYER -----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("11. READING IMAGE AFTER SECOMD CONVOLUTION LAYER"),   UVM_HIGH)
                    cnn_item.input_command  = 14'b01000000000000;
                    //cnn_item.axis_s_valid   = 1'b0;
                    //cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b1;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
        end
        
        if(do_conv_3) begin
                
                //-----------12. RESET IP ------------------
                `uvm_info(get_name(), $sformatf("12. RESET IP"),   UVM_HIGH)  
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00010000000000;
                    //cnn_item.axis_s_valid   = 1'b1;
                    //cnn_item.axis_s_data_in = bias;
                    //cnn_item.axis_s_last    = 1'b1;
                    //cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                //-------------------------------------------------------------------------------------------------------------
                
                // ----------- 13. SENDING QUARTER WEIGHTS_1 -----------
                `uvm_info(get_name(), $sformatf("13. SENDING QUARTER WEIGHTS_1"),   UVM_HIGH)  
                for(int i = 0 ; i < 4608; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000010000000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights2[0*4608+i]);
                    if(i == 4607)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end

                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);

                // ----------- 14. SENDING IMAGE_2 -----------
        `       uvm_info(get_name(), $sformatf("14. SENDING IMAGE_2"),   UVM_HIGH)
                for(int i = 0 ; i < 10368; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000100000000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(image2[i]);
                    if(i == 10367)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 15. START SECOND CONVOLUTION LAYER-----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("15. START THIRD CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00001000000000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 16. SENDING QUARTER WEIGHTS_1 -----------
                `uvm_info(get_name(), $sformatf("16. SENDING QUARTER WEIGHTS_1"),   UVM_HIGH)  
                for(int i = 0 ; i < 4608; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000010000000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights2[1*4608+i]);
                    if(i == 4607)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end

                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 17. START SECOND CONVOLUTION LAYER-----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("17. START THIRD CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00001000000000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 18. SENDING QUARTER WEIGHTS_1 -----------
                `uvm_info(get_name(), $sformatf("18. SENDING QUARTER WEIGHTS_1"),   UVM_HIGH)  
                for(int i = 0 ; i < 4608; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000010000000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights2[2*4608+i]);
                    if(i == 4607)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end

                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 19. START SECOND CONVOLUTION LAYER-----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("19. START THIRD CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00001000000000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 20. SENDING QUARTER WEIGHTS_1 -----------
                `uvm_info(get_name(), $sformatf("20. SENDING QUARTER WEIGHTS_1"),   UVM_HIGH)  
                for(int i = 0 ; i < 4608; i++) 
                begin
                    start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000010000000;
                    cnn_item.axis_s_valid   = 1'b1;
                    cnn_item.axis_s_data_in = real_to_binary(weights2[3*4608+i]);
                    if(i == 4607)
                    begin
                        cnn_item.axis_s_last    = 1'b1;
                    end
                    else begin
                        cnn_item.axis_s_last    = 1'b0;
                    end
                    finish_item(cnn_item);
                end

                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 21. START SECOND CONVOLUTION LAYER-----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("21. START THIRD CONVOLUTION LAYER"),   UVM_HIGH) 
                    cnn_item.input_command  = 14'b00001000000000;
                    cnn_item.axis_s_valid   = 1'b0;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                // ----------- 21. READING IMAGE AFTER THIRD CONVOLUTION LAYER -----------
                start_item(cnn_item);
                    `uvm_info(get_name(), $sformatf("21. READING IMAGE AFTER THIRD CONVOLUTION LAYER"),   UVM_HIGH)
                    cnn_item.input_command  = 14'b10000000000000;
                    //cnn_item.axis_s_valid   = 1'b0;
                    //cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axim_s_ready   = 1'b1;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
                
                //----------- RESET IP ------------------
                `uvm_info(get_name(), $sformatf("RESET IP"),   UVM_HIGH)  
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00010000000000;
                    //cnn_item.axis_s_valid   = 1'b1;
                    //cnn_item.axis_s_data_in = bias;
                    //cnn_item.axis_s_last    = 1'b1;
                    //cnn_item.axim_s_ready   = 1'b0;
                finish_item(cnn_item);
                
                start_item(cnn_item); 
                    cnn_item.input_command  = 14'b00000000000000;
                    cnn_item.axis_s_last    = 1'b0;
                    cnn_item.axis_s_valid   = 1'b0;
                finish_item(cnn_item);
        end
    endtask : body



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
endclass : cnn_simple_seq
`endif
