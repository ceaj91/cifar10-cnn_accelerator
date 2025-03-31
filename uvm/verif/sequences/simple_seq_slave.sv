`ifndef simple_seq_slave_SV
`define simple_seq_slave_SV

class simple_seq_slave extends base_seq_slave;

    `uvm_object_utils(simple_seq_slave)

    integer i = 0;
    seq_item_slave item;
    integer sending_bias;
    integer sending_weigths0, sending_weights1_1, sending_weights1_2;
    integer sending_weights2_1,sending_weights2_2,sending_weights2_3,sending_weights2_4;
    
    integer sending_image0 = 0;
    integer sending_image1 = 0;
    integer sending_image2 = 0;
    
    integer image_number_0 = 0;
    integer image_number_1 = 0;
    integer image_number_2 = 0;

    function new(string name = "seq_item_slave");
        super.new(name);
        extract_data();
    endfunction

    virtual task body();   
        item = seq_item_slave::type_id::create("item");

        //`uvm_info(get_name(), $sformatf("SIZE OF image0_FORMATED %d",image0.size()),UVM_LOW)
        //`uvm_info(get_name(), $sformatf("SIZE OF image1_FORMATED %d",image1.size()),UVM_LOW)
        //`uvm_info(get_name(), $sformatf("SIZE OF image2_FORMATED %d",image2.size()),UVM_LOW)

        if(sending_bias) begin
            while(i < bias.size) begin
                start_item(item);

                assert(item.randomize());
                
                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(bias[i]);           
                
                if(i == bias.size - 1)
                    item.axis_s_last = 1'b1;
                
                if(item.axis_s_valid == 1)
                    i++;

                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)//128
            i = 0;
        end
        sending_bias = 0;
    
        if(sending_weigths0) begin
            while(i < weights0.size) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights0[i]);  
                
                if(i == weights0.size - 1)
                    item.axis_s_last = 1'b1;
                
                if(item.axis_s_valid == 1)
                    i++;
                
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)//864
            i = 0;
        end
        sending_weigths0 = 0;

        if(sending_weights1_1) begin
            while(i<4608) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights1[i]);
                
                if(i == 4607) begin
                    item.axis_s_last = 1'b1;
                end 

                if(item.axis_s_valid == 1) begin
                    i++;
                end
                    
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
        end
        sending_weights1_1 = 0;

        if(sending_weights1_2) begin
            while(i<4608) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights1[4608+i]);
                
                if(i == 4607)
                    item.axis_s_last = 1'b1;

                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
        end
        sending_weights1_2 = 0;

        if(sending_weights2_1) begin
            while(i<4608) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights2[i]);
                
                if(i == 4607)
                    item.axis_s_last = 1'b1;

                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
        end
        sending_weights2_1 = 0;

        if(sending_weights2_2) begin
            while(i<4608) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights2[4608*1+i]);
                
                if(i == 4607)
                    item.axis_s_last = 1'b1;

                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
        end
        sending_weights2_2 = 0;


        if(sending_weights2_3) begin
            while(i<4608) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights2[4608*2+i]);
                
                if(i == 4607)
                    item.axis_s_last = 1'b1;

                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
        end
        sending_weights2_3 = 0;

        if(sending_weights2_4) begin
            while(i<4608) begin
                start_item(item);
                
                assert(item.randomize());

                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(weights2[4608*3+i]);
                
                if(i == 4607)
                    item.axis_s_last = 1'b1;

                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
        end
        sending_weights2_4 = 0;

        if(sending_image0) begin //3468
            while(i < 3468) begin
                start_item(item);
                
                assert(item.randomize());
                
                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(image0[image_number_0 + i]);
                
                if(i == 3468 - 1)
                    item.axis_s_last = 1'b1;
                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
            image_number_0 = 0;
            sending_image0 = 0;
        end
        
        // COPY-PASTE iz prethodnog, mozda ne radi
        if(sending_image1) begin
            while(i < 10368) begin
                start_item(item);
                
                assert(item.randomize());
                
                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(image1[image_number_1 + i]);
                
                if(i == 10368 - 1)
                    item.axis_s_last = 1'b1;
                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
            image_number_1 = 0;
            sending_image1 = 0;
        end
        
        // COPY-PASTE iz prethodnog, mozda ne radi
        if(sending_image2) begin
            while(i < 3200) begin
                start_item(item);
                
                assert(item.randomize());
                
                item.axis_s_last    = 1'b0;
                item.axis_s_data_in = real_to_binary(image2[image_number_2 + i]);
                
                if(i == 3200 - 1)
                    item.axis_s_last = 1'b1;
                if(item.axis_s_valid == 1)
                    i++;
                finish_item(item);
            end
            `uvm_info(get_name(), $sformatf("i = %d    ",i),UVM_HIGH)
            i = 0;
            image_number_2 = 0;
            sending_image2 = 0;
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

endclass : simple_seq_slave
`endif


/*
CONV0 LAYER:
-picture size to send: 34x34x3
-weights to send : 3x3x3x32
-output : 32x32x32

CONV1 LAYER:
-picture size to send: 18x18x32
-weights to send : 3x3x32x16   +   3x3x32x16  
-output : 16x16x32

CONV2 LAYER:
-picture size to send: 10x10x32
-weights to send : 3x3x32x16   +   3x3x32x16 + 3x3x32x16   +   3x3x32x16  
-output : 8x8x64
*/