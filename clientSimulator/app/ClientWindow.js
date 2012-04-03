Ext.define('ClientWindow', {
	extend: 'Ext.window.Window',
    id:'ClientWindow',

    height: 713,
    width: 644,

    layout: {
        type: 'absolute'
    },
    title: 'Client Simulator',

    initComponent: function() {
        var me = this;

        Ext.applyIf(me, {
        	items: [
                    {
                        xtype: 'form',
                        height: 300,
                        width: 280,
                        bodyPadding: 10,
                        collapsible: true,
                        title: 'Sender',
                        x: 20,
                        y: 20,
                        items: [
                            {
                            	id:'sender.senderMDN',
                                xtype: 'textfield',
                                fieldLabel: 'Sender MDN',
                                anchor: '100%'
                            },
                            {
                            	id:'sender.recipientMDN',
                                xtype: 'textfield',
                                fieldLabel: 'Recipient MDN',
                                anchor: '100%'
                            },
                            {	id:'sender.message',
                                xtype: 'textareafield',
                                height: 158,
                                fieldLabel: 'SMS Message',
                                anchor: '100%'
                            },
                            {
                                xtype: 'button',
                                text: 'Send',
                                anchor: '100%',
                                handler: me.sendRequest
                            }
                        ]
                    },
                    {
                        xtype: 'form',
                        height: 300,
                        width: 280,
                        bodyPadding: 10,
                        collapsible: true,
                        title: 'Reciever',
                        x: 20,
                        y: 360,
                        items: [
                            {
                            	id:'recipent1.senderMDN',
                                xtype: 'textfield',
                                readOnly: true,
                                fieldLabel: 'Sender MDN',
                                anchor: '100%'
                            },
                            {
                            	id:'recipent1.recipentMDN',
                                xtype: 'textfield',
                                fieldLabel: 'Recipient MDN',
                                anchor: '100%'
                            },
                            {
                            	id:'recipent1.message',
                                xtype: 'textareafield',
                                height: 158,
                                fieldLabel: 'SMS Message',
                                anchor: '100%'
                            }
                        ]
                    },
                    {
                        xtype: 'form',
                        height: 300,
                        width: 280,
                        bodyPadding: 10,
                        collapsible: true,
                        title: 'Reciever',
                        x: 330,
                        y: 360,
                        items: [
                            {
                            	id:'recipent2.senderMDN',
                                xtype: 'textfield',
                                readOnly: true,
                                fieldLabel: 'Sender MDN',
                                anchor: '100%'
                            },
                            {
                            	id:'recipent2.recipentMDN',
                                xtype: 'textfield',
                                fieldLabel: 'Recipient MDN',
                                anchor: '100%'
                            },
                            {
                            	id:'recipent2.message',
                                xtype: 'textareafield',
                                height: 158,
                                fieldLabel: 'SMS Message',
                                anchor: '100%'
                            }
                        ]
                    }
                ]
        });

        //me.mask = new Ext.LoadMask(me.getEl(),{msg:'Loading..'});
        //me.mask = new Ext.LoadMask(Ext.getCmp('ClientWindow').getEl(),{msg:'Sending..'});
        me.mask = new Ext.LoadMask(Ext.getBody(), {msg:"Please wait..."});

        me.callParent(arguments);
    },

    sendRequest : function(){
    	var me = this;

    	Ext.getCmp('ClientWindow').mask.show();

    	var hostURL = 'http://newbiz.uangel.com/skytel/';
//    	var hostURL = 'http://192.168.1.156/';

    	var sendParam = {
    			sender_phoneno: Ext.getCmp('sender.senderMDN').value,
//    			recipient_phoneno_list: [Ext.getCmp('sender.recipientMDN').value],
    			recipient_phoneno_list: Ext.getCmp('sender.recipientMDN').value,
    			body: Ext.getCmp('sender.message').value
    	};

    	Ext.Ajax.request({
		    url: hostURL + 'note/privatemsg/send_by_phoneno',
		    method: 'POST',
		    params: sendParam,
		    success: function(resp){
		    	console.log('--send--');
		    	console.log(resp);

		    	var responseText = Ext.decode(resp.responseText);

		    	if(responseText['success']){
		    		var rMDN = Ext.getCmp('sender.recipientMDN').value;

			    	if(rMDN == '1111' || rMDN == '1112' || rMDN == '1113'){
			    		var senderParam1 = {
			    				recipient_phoneno: Ext.getCmp('sender.senderMDN').value
			    		};

			    		Ext.Ajax.request({
			    		    url: hostURL + 'note/privatemsg/get_by_phoneno',
			    		    method: 'POST',
			    		    params: senderParam1,
			    		    success: function(resp){
			    		    	var responseText = Ext.decode(resp.responseText);
			    		    	console.log('--sender--');
			    		    	console.log(resp);
			    		    	if(responseText != 'FALSE'){
			    		    		Ext.getCmp('sender.message').setValue(responseText.body);
			    		    	}
			    		    },
			    		    failure:function(reqs){
			    		    	console.log(reqs);
			    		    	Ext.MessageBox.alert('Error occured during connection..');
			    			}
			    		});
			    	}

		    		//client 1
		    		var readParam1 = {
		    				recipient_phoneno: Ext.getCmp('recipent1.recipentMDN').value
		    		};

		    		Ext.Ajax.request({
		    		    url: hostURL + 'note/privatemsg/get_by_phoneno',
		    		    method: 'POST',
		    		    params: readParam1,
		    		    success: function(resp){
		    		    	var responseText = Ext.decode(resp.responseText);
		    		    	console.log('--read1--');
		    		    	console.log(resp);
		    		    	if(responseText != 'FALSE'){
		    		    		Ext.getCmp('recipent1.senderMDN').setValue(responseText.sender_phoneno);
		    		    		Ext.getCmp('recipent1.message').setValue(responseText.body);
		    		    	}
		    		    },
		    		    failure:function(reqs){
		    		    	console.log(reqs);
		    		    	Ext.MessageBox.alert('Error occured during connection..');
		    			}
		    		});

		    		//client 2
	    			var readParam2 = {
		    				recipient_phoneno: Ext.getCmp('recipent2.recipentMDN').value
		    		};

		    		Ext.Ajax.request({
		    		    url: hostURL + 'note/privatemsg/get_by_phoneno',
		    		    method: 'POST',
		    		    params: readParam2,
		    		    success: function(resp){
		    		    	var responseText = Ext.decode(resp.responseText);
		    		    	console.log('--read2--');
		    		    	console.log(resp);
		    		    	if(responseText != 'FALSE'){
		    		    		Ext.getCmp('recipent2.senderMDN').setValue(responseText.sender_phoneno);
		    		    		Ext.getCmp('recipent2.message').setValue(responseText.body);
		    		    	}
		    		    },
		    		    failure:function(reqs){
		    		    	console.log(reqs);
		    		    	Ext.MessageBox.alert('Error occured during connection..');
		    			}
		    		});
		    	}
		    	else{
		    		Ext.MessageBox.alert('Sending error..' +  responseText);
		    	}
		    	Ext.getCmp('ClientWindow').mask.hide();
		    },
		    failure:function(resp){
		    	console.log(resp);
		    	Ext.MessageBox.alert('Error occured during connection..');
		    	Ext.getCmp('ClientWindow').mask.hide();
			}
    	});
    },

});

