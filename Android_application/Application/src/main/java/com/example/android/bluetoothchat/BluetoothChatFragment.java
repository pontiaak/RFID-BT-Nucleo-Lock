/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.example.android.bluetoothchat;

import android.Manifest;
import android.app.ActionBar;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.*;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;

import com.example.android.Config;
import com.example.android.common.logger.Log;

import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import android.widget.PopupWindow;
import android.graphics.drawable.ColorDrawable;
import android.content.Context;
import android.view.LayoutInflater;
import android.app.AlertDialog;
import android.text.InputType;
import android.content.DialogInterface;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.nio.charset.StandardCharsets;
import android.util.Base64;
import java.util.Base64.Decoder;
import java.math.BigInteger;
import java.security.PublicKey;
import java.security.spec.RSAPublicKeySpec;
import java.security.KeyFactory;
import java.security.SecureRandom;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;


/**
 * This fragment controls Bluetooth to communicate with other devices.
 */
public class BluetoothChatFragment extends Fragment {

    private boolean isPasswordCorrect = false;

    private static final String TAG = "BluetoothChatFragment";

    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE_SECURE = 1;
    private static final int REQUEST_CONNECT_DEVICE_INSECURE = 2;
    private static final int REQUEST_ENABLE_BT = 3;

    // Layout Views
    private Button openDoor, readToggle;
    private TextView textViewReceived;
    private EditText editText;
    private Button synchUsersButton;
    private Button addUserButton;
    private Button deleteUserButton;
    private Button history;
    private String receiveBuffer = "";
    private Button synchTime;
    private Button writeToggle;
    /**
     * Name of the connected device
     */
    private String mConnectedDeviceName = null;
    /**
     * The Handler that gets information back from the BluetoothChatService
     */
    private final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            FragmentActivity activity = getActivity();
            switch (msg.what) {
                case Constants.MESSAGE_STATE_CHANGE:
                    switch (msg.arg1) {
                        case BluetoothChatService.STATE_CONNECTED:
                            setStatus(getString(R.string.title_connected_to, mConnectedDeviceName));
                            break;
                        case BluetoothChatService.STATE_CONNECTING:
                            setStatus(R.string.title_connecting);
                            break;
                        case BluetoothChatService.STATE_LISTEN:
                        case BluetoothChatService.STATE_NONE:
                            setStatus(R.string.title_not_connected);
                            break;
                    }
                    break;
                case Constants.MESSAGE_WRITE:
                    break;
                case Constants.MESSAGE_READ:
                    byte[] readBuf = (byte[]) msg.obj;
                    // construct a string from the valid bytes in the buffer
                    String readMessage = new String(readBuf, 0, msg.arg1);
                    receiveBuffer += readMessage;
                    if (receiveBuffer.contains("\n")) {
                        receiveBuffer = receiveBuffer.substring(0, receiveBuffer.length() - 1);
                        messageHandler();
                        receiveBuffer = "";
                    }
                    break;
                    case Constants.MESSAGE_DEVICE_NAME:
                    // save the connected device's name
                    mConnectedDeviceName = msg.getData().getString(Constants.DEVICE_NAME);
                    if (null != activity) {
                        Toast.makeText(activity, "Connected to "
                                + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                    }
                    break;
                case Constants.MESSAGE_TOAST:
                    if (null != activity) {
                        Toast.makeText(activity, msg.getData().getString(Constants.TOAST),
                                Toast.LENGTH_SHORT).show();
                    }
                    break;
            }
        }
    };
    /**
     * String buffer for outgoing messages
     */
    private StringBuffer mOutStringBuffer;
    /**
     * Local Bluetooth adapter
     */
    private BluetoothAdapter mBluetoothAdapter = null;
    /**
     * Member object for the chat services
     */
    private BluetoothChatService mChatService = null;
    /**
     * The action listener for the EditText widget, to listen for the return key
     */
    private TextView.OnEditorActionListener mWriteListener
            = new TextView.OnEditorActionListener() {
        public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
            // If the action is a key-up event on the return key, send the message
            if (actionId == EditorInfo.IME_NULL && event.getAction() == KeyEvent.ACTION_UP) {
                String message = view.getText().toString();
                sendMessage(message);
            }
            return true;
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setHasOptionsMenu(true);
        // Get local Bluetooth adapter
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        // If the adapter is null, then Bluetooth is not supported
        FragmentActivity activity = getActivity();
        if (mBluetoothAdapter == null && activity != null) {
            Toast.makeText(activity, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            activity.finish();
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        if (mBluetoothAdapter == null) {
            return;
        }
        // If BT is not on, request that it be enabled.
        // setupChat() will then be called during onActivityResult
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
            // Otherwise, setup the chat session
        } else if (mChatService == null) {
            setupChat();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (mChatService != null) {
            mChatService.stop();
        }
    }

    @Override
    public void onResume() {
        super.onResume();

        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mChatService != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mChatService.getState() == BluetoothChatService.STATE_NONE) {
                // Start the Bluetooth chat services
                mChatService.start();
            }
        }
    }

    private void encryptAndSendMessage(String message){
        BigInteger[] encrypted = rsaEncrypt(message, pubSTM);
        // Concatenate BigIntegers into a single String
        StringBuilder messageBuilder = new StringBuilder();
        for (BigInteger bigInteger : encrypted) {
            String paddedBigIntString = String.format("%010d", bigInteger); //sometimes when the element has 0 at the start we have to add a 0 to it, because otherwise the stm has no way of knowing (we sent them glued-up together)
            messageBuilder.append(paddedBigIntString);
            //messageBuilder.append(" ");
        }
        String encryptedString = messageBuilder.toString();
        String originalMessage = "m:" + encryptedString + "}";
        int spacesToAdd = 500 - originalMessage.length();
        StringBuilder paddedMessage = new StringBuilder(originalMessage);
        for (int i = 0; i < spacesToAdd; i++) {
            paddedMessage.append(" ");
        }
        sendMessage(paddedMessage.toString());
    }


    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_bluetooth_chat, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        editText = view.findViewById(R.id.editText);
        synchUsersButton = view.findViewById(R.id.synchUsers);
        addUserButton = view.findViewById(R.id.addUser);
        deleteUserButton = view.findViewById(R.id.deleteUser);
        textViewReceived = view.findViewById(R.id.textViewReceived);
        readToggle = view.findViewById(R.id.readToggle);
        history = view.findViewById(R.id.history);
        writeToggle = view.findViewById(R.id.writeToggle);
        synchTime = view.findViewById(R.id.synchTime);
        openDoor = view.findViewById(R.id.openDoor);

        readToggle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (readToggle.getText().equals("Read On")) {
                    updateButtonState("read", "0");
                    /*readToggle.setText("Read Off");
                    readToggle.setBackgroundColor(Color.RED);*/
                    //sendMessage(Config.getLastString(Config.READ_TOGGLE_OFF));
                    String messageToEncrypt = "readOff";
                    encryptAndSendMessage(messageToEncrypt);
                } else {
                    updateButtonState("read", "1");
                    updateButtonState("write", "0");
                    /*readToggle.setText("Read On");
                    readToggle.setBackgroundColor(Color.GREEN);*/
                    //sendMessage(Config.getLastString(Config.READ_TOGGLE_ON));
                    String messageToEncrypt = "readOn";
                    encryptAndSendMessage(messageToEncrypt);
                }
            }
        });

        openDoor.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                //sendMessage(Config.getLastString(Config.OPEN_DOOR));

                String messageToEncrypt = "openDoor";
                encryptAndSendMessage(messageToEncrypt);

            }
        });


        synchUsersButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //sendMessage(Config.getLastString(Config.SYNCH_USER));
                String messageToEncrypt = "syncUser";
                encryptAndSendMessage(messageToEncrypt);
            }
        });

        history.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //sendMessage(Config.getLastString(Config.HISTORY));
                String messageToEncrypt = "history";
                encryptAndSendMessage(messageToEncrypt);
            }
        });

        addUserButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Handle addUserButton click
                String userName = editText.getText().toString();
                if (!userName.isEmpty()) {

                    /*String originalMessage = "{Add:" + userName + "}";
                    int spacesToAdd = 500 - originalMessage.length();
                    StringBuilder paddedMessage = new StringBuilder(originalMessage);
                    for (int i = 0; i < spacesToAdd; i++) {
                        paddedMessage.append(" ");
                    }
                    sendMessage(paddedMessage.toString());*/

                    String messageToEncrypt = "add:" + userName + "}";
                    encryptAndSendMessage(messageToEncrypt);

                }
            }
        });


        deleteUserButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Handle deleteUserButton click
                String userName = editText.getText().toString();
                if (!userName.isEmpty()) {

                    /*String originalMessage = "{Dlt:" + userName + "}";
                    int spacesToAdd = 500 - originalMessage.length();
                    StringBuilder paddedMessage = new StringBuilder(originalMessage);
                    for (int i = 0; i < spacesToAdd; i++) {
                        paddedMessage.append(" ");
                    }
                    sendMessage(paddedMessage.toString());
                    */
                    String messageToEncrypt = "Dlt:" + userName + "}";
                    encryptAndSendMessage(messageToEncrypt);

                }
            }
        });

        writeToggle.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String userName = editText.getText().toString();

                if (writeToggle.getText().equals("Write On")) {
                    updateButtonState("write", "0");
                            /*writeToggle.setText("Write Off");
                            writeToggle.setBackgroundColor(Color.RED);*/
                    //sendMessage(Config.getLastString(Config.WRITE_TOGGLE_OFF));
                    String messageToEncrypt = "writeOff";
                    encryptAndSendMessage(messageToEncrypt);
                } else {
                    if (!userName.isEmpty()) {
                        updateButtonState("read", "0");
                        updateButtonState("write", "1");
                    /*writeToggle.setText("Write On");
                    writeToggle.setBackgroundColor(Color.GREEN);*/

                        /*String originalMessage = "{wOn:" + userName + "}";
                        int spacesToAdd = 500 - originalMessage.length();
                        StringBuilder paddedMessage = new StringBuilder(originalMessage);
                        for (int i = 0; i < spacesToAdd; i++) {
                            paddedMessage.append(" ");
                        }
                        sendMessage(paddedMessage.toString());*/
                        String messageToEncrypt = "wOn:" + userName + "}";
                        encryptAndSendMessage(messageToEncrypt);

                    }
                }

            }
        });
        synchTime.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss/dd-MM-yy", Locale.getDefault());
                String currentTime = sdf.format(new Date());
                String originalMessage = "{time:" + currentTime + "}";
                int spacesToAdd = 500 - originalMessage.length();
                StringBuilder paddedMessage = new StringBuilder(originalMessage);
                for (int i = 0; i < spacesToAdd; i++) {
                    paddedMessage.append(" ");
                }
                sendMessage(paddedMessage.toString());


                Handler handler2 = new Handler();
                handler2.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        rsaGenKeys(pub, priv);
                        String publicKeyString = formatPublicKey(pub);
                        String originalMessage = publicKeyString;
                        int spacesToAdd = 500 - originalMessage.length();
                        StringBuilder paddedMessage = new StringBuilder(originalMessage);
                        for (int i = 0; i < spacesToAdd; i++) {
                            paddedMessage.append(" ");
                        }
                        sendMessage(paddedMessage.toString());

                    }
                }, 1000); // 500 milliseconds (half a second)
            }
        });


    }

    /**
     * Set up the UI and background operations for chat.
     */
    private void setupChat() {
        Log.d(TAG, "setupChat()");

        // Initialize the array adapter for the conversation thread
        FragmentActivity activity = getActivity();
        if (activity == null) {
            return;
        }


        // Initialize the compose field with a listener for the return key

        // Initialize the send button with a listener that for click events
        /*mSendButton.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                // Send a message using content of the edit text widget

                View view = getView();
                if (null != view) {
                    TextView textView = view.findViewById(R.id.edit_text_out);
                    String message = textView.getText().toString();
                    if (message.length() > 0) {
                        sendMessage(message);
                        mOutEditText.setText("");
                    }
                }
            }
        });*/

        // Initialize the BluetoothChatService to perform bluetooth connections
        mChatService = new BluetoothChatService(activity, mHandler);

        // Initialize the buffer for outgoing messages
        mOutStringBuffer = new StringBuffer();
    }


    /**
     * Makes this device discoverable for 300 seconds (5 minutes).
     */
    private static final int REQUEST_DISCOVERABLE = 2;
    private void ensureDiscoverable() {
        if (ActivityCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED
                || ActivityCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH_ADMIN) != PackageManager.PERMISSION_GRANTED) {
            // Request Bluetooth permissions if not granted
            ActivityCompat.requestPermissions(requireActivity(),
                    new String[]{Manifest.permission.BLUETOOTH, Manifest.permission.BLUETOOTH_ADMIN},
                    REQUEST_ENABLE_BT);
            return;
        }

        if (mBluetoothAdapter.getScanMode() != BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            Intent discoverableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
            discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
            startActivityForResult(discoverableIntent, REQUEST_DISCOVERABLE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_ENABLE_BT) {
            // Check if the Bluetooth permissions are granted
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED
                    && grantResults.length > 1 && grantResults[1] == PackageManager.PERMISSION_GRANTED) {
                // Permissions granted, try enabling discoverability again
                ensureDiscoverable();
            } else {
                // Permissions not granted, handle accordingly
                // You may want to show a message to the user indicating that Bluetooth features won't work
            }
        }
    }

    /**
     * Sends a message.
     *
     * @param message A string of text to send.
     */
    public void sendMessage(String message) {
        // Check that we're actually connected before trying anything
        if (mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(getActivity(), R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = message.getBytes();
            mChatService.write(send);

            // Reset out string buffer to zero and clear the edit text field
            mOutStringBuffer.setLength(0);
            editText.setText("");  // Clear the edit text field after sending the message
        }
    }

    /**
     * Updates the status on the action bar.
     *
     * @param resId a string resource ID
     */
    private void setStatus(int resId) {
        FragmentActivity activity = getActivity();
        if (null == activity) {
            return;
        }
        final ActionBar actionBar = activity.getActionBar();
        if (null == actionBar) {
            return;
        }
        actionBar.setSubtitle(resId);
    }

    /**
     * Updates the status on the action bar.
     *
     * @param subTitle status
     */
    private void setStatus(CharSequence subTitle) {
        FragmentActivity activity = getActivity();
        if (null == activity) {
            return;
        }
        final ActionBar actionBar = activity.getActionBar();
        if (null == actionBar) {
            return;
        }
        actionBar.setSubtitle(subTitle);
    }

    BigInteger exponent = null;  //public Exponent of rsa key for encryption/decryption
    BigInteger modulus = null;   //public Modulus of rsa key for encryption/decryption
    String encodedMessage = "";
    PublicKey pub = new PublicKey(BigInteger.ZERO, BigInteger.ZERO);
    PrivateKey priv = new PrivateKey(BigInteger.ZERO, BigInteger.ZERO);
    PublicKey pubSTM = new PublicKey(BigInteger.ZERO, BigInteger.ZERO);
    private StringBuilder historyStringBuilder = new StringBuilder();
    private StringBuilder usersStringBuilder = new StringBuilder();
    private void messageHandler(){     //handles incoming messages



        if (receiveBuffer != null) {
            //textViewReceived.setText(receiveBuffer);    //important

            if (receiveBuffer.startsWith("pass_correct")) {
                isPasswordCorrect = true;
                if (receiveBuffer.startsWith("pass_correct,read:1,write:0")) {updateButtonState("read", "1");updateButtonState("write", "0");}
                if (receiveBuffer.startsWith("pass_correct,read:1,write:1")) {updateButtonState("read", "1");updateButtonState("write", "1");}
                if (receiveBuffer.startsWith("pass_correct,read:0,write:1")) {updateButtonState("read", "0");updateButtonState("write", "1");}
                if (receiveBuffer.startsWith("pass_correct,read:0,write:0")) {updateButtonState("read", "0");updateButtonState("write", "0");}
                //SENDING COORECT TIME TO THE STM, the same as "time" button
                Handler handler = new Handler();
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss/dd-MM-yy", Locale.getDefault());
                        String currentTime = sdf.format(new Date());
                        String originalMessage = "{time:" + currentTime + "}";
                        int spacesToAdd = 500 - originalMessage.length();
                        StringBuilder paddedMessage = new StringBuilder(originalMessage);
                        for (int i = 0; i < spacesToAdd; i++) {
                            paddedMessage.append(" ");
                        }
                        sendMessage(paddedMessage.toString());
                    }
                }, 0); // 500 milliseconds (half a second)

                //GENERATING RSA KEYS and sending them over to the stm in format of {e1:131073e2:m1:401025m2:1639}000000000000000000
                /*Handler handler2 = new Handler();
                handler2.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        rsaGenKeys(pub, priv);
                        String publicKeyString = formatPublicKey(pub);
                        String originalMessage = publicKeyString;
                        int spacesToAdd = 500 - originalMessage.length();
                        StringBuilder paddedMessage = new StringBuilder(originalMessage);
                        for (int i = 0; i < spacesToAdd; i++) {
                            paddedMessage.append(" ");
                        }
                        sendMessage(paddedMessage.toString());

                    }
                }, 1000); // 500 milliseconds (half a second)*/




            } else if (receiveBuffer.startsWith("pass_incorrect")) {
                /*Toast.makeText(getActivity(), "Incorrect password", Toast.LENGTH_SHORT).show();
                showPasswordDialog();  // Show the dialog again*/
            }
            // Check if the received message is related to user management
            else if (receiveBuffer.startsWith("usr:")) {
                /*// Display the user list in a popup window
                showUserListPopup(receiveBuffer.substring(4)); // Assuming "usr:" is followed by the user list*/
            }else if(receiveBuffer.startsWith("hist:")) {
                // Handle history messages by showing them in a pop-up window
                showHistoryPopup(receiveBuffer.substring(5));


            }else if(receiveBuffer.startsWith("key")) {
                //this gets the public key from stm for later encoding data for sending it back
                int startIndexE = receiveBuffer.indexOf("e:") + 2;
                int endIndexE = receiveBuffer.indexOf("m:");
                int startIndexM = endIndexE + 2;
                // Extract the substrings containing the numbers
                String eValue = receiveBuffer.substring(startIndexE, endIndexE);
                String mValue = receiveBuffer.substring(startIndexM);

                // Remove non-numeric characters from mValue        (remowing } from the end)
                mValue = mValue.replaceAll("[^\\d]", "");

                // Convert the substrings to BigIntegers
                BigInteger eBigInteger = new BigInteger(eValue);
                BigInteger mBigInteger = new BigInteger(mValue);

                // Create PublicKey with the extracted values
                pubSTM.modulus = mBigInteger;
                pubSTM.exponent = eBigInteger;
                textViewReceived.setText("STM Public key modulus:" + pubSTM.modulus + " exponent: " + pubSTM.exponent);


            }else if (receiveBuffer.startsWith("pe:")) {
                // Extract public exponent
                String exponentString = receiveBuffer.substring(3);
                exponent = new BigInteger(exponentString);

            } else if (receiveBuffer.startsWith("pm:")) {
                // Extract public modulus
                String modulusString = receiveBuffer.substring(3);
                modulus = new BigInteger(modulusString);

            } else if (receiveBuffer.startsWith("e:")) {

                // Extract the string after 'e:'
                String encryptedString = receiveBuffer.substring(receiveBuffer.indexOf("e:") + 2);

                // Split the string into an array of strings
                String[] encryptedArray = encryptedString.trim().split("\\s+");

                // Convert the array of strings to an array of BigIntegers
                BigInteger[] encryptedBigIntegers = new BigInteger[encryptedArray.length];
                for (int i = 0; i < encryptedArray.length; i++) {
                    encryptedBigIntegers[i] = new BigInteger(encryptedArray[i]);
                }



                String message = "hello, Naomi! (from android)";
                BigInteger[] encrypted = rsaEncrypt(message, pub);
                String decrypted = rsaDecrypt(encryptedBigIntegers, priv);
                textViewReceived.setText("Encrypted message: " + Arrays.toString(encryptedBigIntegers) + "Decrypted message: " + decrypted);

                if (decrypted.equals("HistoryStart")) {
                    // Clear the StringBuilder when HistoryStart is received
                    historyStringBuilder.setLength(0);
                } else if (decrypted.equals("HistoryEnd")) {
                    // When HistoryEnd is received, show the history popup
                    showHistoryPopup(historyStringBuilder.toString());
                } else if (decrypted.startsWith("h:")) {
                    // Extract the username and timestamp from the message
                    String historyMessage = decrypted.substring(2); // Remove "h:"
                    historyStringBuilder.append(historyMessage).append("\n");
                }

                if (decrypted.equals("UsersStart")) {
                    // Clear the StringBuilder when HistoryStart is received
                    usersStringBuilder.setLength(0);
                } else if (decrypted.equals("UsersEnd")) {
                    // When HistoryEnd is received, show the history popup
                    showUserListPopup(usersStringBuilder.toString());
                } else if (decrypted.startsWith("u:")) {
                    // Extract the username and timestamp from the message
                    String usersMessage = decrypted.substring(2); // Remove "h:"
                    usersStringBuilder.append(usersMessage).append("\n");
                } else if (decrypted.startsWith("pass_correct")) {
                    isPasswordCorrect = true;
                    if (decrypted.startsWith("pass_correct,read:1,write:0")) {
                        updateButtonState("read", "1");
                        updateButtonState("write", "0");
                    }
                    if (decrypted.startsWith("pass_correct,read:1,write:1")) {
                        updateButtonState("read", "1");
                        updateButtonState("write", "1");
                    }
                    if (decrypted.startsWith("pass_correct,read:0,write:1")) {
                        updateButtonState("read", "0");
                        updateButtonState("write", "1");
                    }
                    if (decrypted.startsWith("pass_correct,read:0,write:0")) {
                        updateButtonState("read", "0");
                        updateButtonState("write", "0");
                    }
                    //SENDING COORECT TIME TO THE STM, the same as "time" button
                    Handler handler = new Handler();
                    handler.postDelayed(new Runnable() {
                        @Override
                        public void run() {
                            SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss/dd-MM-yy", Locale.getDefault());
                            String currentTime = sdf.format(new Date());
                            String originalMessage = "{time:" + currentTime + "}";
                            int spacesToAdd = 500 - originalMessage.length();
                            StringBuilder paddedMessage = new StringBuilder(originalMessage);
                            for (int i = 0; i < spacesToAdd; i++) {
                                paddedMessage.append(" ");
                            }
                            sendMessage(paddedMessage.toString());
                        }
                    }, 0); // 500 milliseconds (half a second)
                }
            }
            // will add more cases if needed

            receiveBuffer = "";
        }
    }


    public static class PublicKey {
        public BigInteger modulus;
        public BigInteger exponent;

        public PublicKey(BigInteger modulus, BigInteger exponent) {
            this.modulus = modulus;
            this.exponent = exponent;
        }
    }

    public static class PrivateKey {
        public BigInteger modulus;
        public BigInteger exponent;

        public PrivateKey(BigInteger modulus, BigInteger exponent) {
            this.modulus = modulus;
            this.exponent = exponent;
        }
    }

    public static void rsaGenKeys(PublicKey pub, PrivateKey priv) {
        SecureRandom random = new SecureRandom();

        // Choose random prime numbers p and q
        BigInteger p = BigInteger.valueOf(13163);//pairs with one lower: 45161 or 13163
        BigInteger q = BigInteger.valueOf(15803);//pairs with one above: 88799 or 15803

        // Calculate n (modulus) and phi
        BigInteger modulus = p.multiply(q);
        BigInteger phi = p.subtract(BigInteger.ONE).multiply(q.subtract(BigInteger.ONE));

        // Choose a public exponent e
        BigInteger e = BigInteger.valueOf((2 << 16) + 1);

        // Calculate private exponent d
        BigInteger d = ExtEuclid(phi, e);
        while (d.compareTo(BigInteger.ZERO) < 0) {
            d = d.add(phi);
        }

        // Set the generated values to pub and priv
        pub.modulus = modulus;
        pub.exponent = e;

        priv.modulus = modulus;
        priv.exponent = d;
    }

    public static BigInteger[] rsaEncrypt(String message, PublicKey pub) {
        BigInteger[] encrypted = new BigInteger[message.length()];
        BigInteger max = pub.modulus;
        BigInteger e = pub.exponent;

        for (int i = 0; i < message.length(); i++) {
            BigInteger m = BigInteger.valueOf(message.charAt(i));
            BigInteger c = m.modPow(e, max);
            encrypted[i] = c;
        }

        return encrypted;
    }

    public static String rsaDecrypt(BigInteger[] encrypted, PrivateKey priv) {
        BigInteger[] decrypted = new BigInteger[encrypted.length];
        BigInteger max = priv.modulus;
        BigInteger d = priv.exponent;

        for (int i = 0; i < encrypted.length; i++) {
            BigInteger c = encrypted[i];
            BigInteger m = c.modPow(d, max);
            decrypted[i] = m;
        }

        char[] decryptedChars = new char[decrypted.length];
        for (int i = 0; i < decrypted.length; i++) {
            decryptedChars[i] = (char) decrypted[i].intValue();
        }

        return new String(decryptedChars);
    }

    public static BigInteger ExtEuclid(BigInteger a, BigInteger b) {
        BigInteger x = BigInteger.ZERO;
        BigInteger y = BigInteger.ONE;
        BigInteger u = BigInteger.ONE;
        BigInteger v = BigInteger.ZERO;
        BigInteger gcd = b;

        while (!a.equals(BigInteger.ZERO)) {
            BigInteger[] quotientAndRemainder = gcd.divideAndRemainder(a);
            BigInteger q = quotientAndRemainder[0];
            BigInteger r = quotientAndRemainder[1];

            BigInteger m = x.subtract(u.multiply(q));
            BigInteger n = y.subtract(v.multiply(q));

            gcd = a;
            a = r;
            x = u;
            y = v;
            u = m;
            v = n;
        }

        return y;
    }

    public static String formatPublicKey(PublicKey pub) {
        // Convert public exponent and modulus to strings
        String exponentStr = pub.exponent.toString();
        String modulusStr = pub.modulus.toString();

        // Split the exponent and modulus into two parts
        String e1 = exponentStr.substring(0, exponentStr.length() / 2);
        String e2 = exponentStr.substring(exponentStr.length() / 2);

        String m1 = modulusStr.substring(0, modulusStr.length() / 2);
        String m2 = modulusStr.substring(modulusStr.length() / 2);

        // Create the formatted string
        return "{e1:" + e1 + "e2:" + e2 + "m1:" + m1 + "m2:" + m2 + "}";
    }


    private void showUserListPopup(String userList) {
        if (getActivity() == null) {
            // Handle the case where the fragment is not attached to an activity
            return;
        }
        LayoutInflater inflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (inflater == null) {
            // Handle the case where the inflater is not available
            return;
        }
        View popupView = inflater.inflate(R.layout.popup_user_list, null);

        TextView userListTextView = popupView.findViewById(R.id.userListTextView);
        userListTextView.setText(userList);

        PopupWindow popupWindow = new PopupWindow(
                popupView,
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT,
                true
        );

        // Set background color for the pop-up window
        popupWindow.setBackgroundDrawable(new ColorDrawable(Color.WHITE));

        // Show the pop-up window
        popupWindow.showAtLocation(popupView, Gravity.CENTER, 0, 0);
    }


    private void showHistoryPopup(String historyMessage) {
        if (getActivity() == null) {
            // Handle the case where the fragment is not attached to an activity
            return;
        }

        LayoutInflater inflater = (LayoutInflater) getActivity().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (inflater == null) {
            // Handle the case where the inflater is not available
            return;
        }

        View popupView = inflater.inflate(R.layout.popup_history, null);

        TextView historyTextView = popupView.findViewById(R.id.historyTextView);
        historyTextView.setText(historyMessage);

        PopupWindow popupWindow = new PopupWindow(
                popupView,
                ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT,
                true
        );

        // Set background color for the pop-up window
        popupWindow.setBackgroundDrawable(new ColorDrawable(Color.WHITE));

        // Show the pop-up window
        popupWindow.showAtLocation(popupView, Gravity.CENTER, 0, 0);
    }

    private void updateButtonState(String key, String value) {
        switch (key) {
            case "read":
                int readState = Integer.parseInt(value);
                setReadButtonState(readState);
                break;

            case "write":
                int writeState = Integer.parseInt(value);
                setWriteButtonState(writeState);
                break;

            // Add more cases for other keys as needed

            default:
                Log.w(TAG, "Unknown key: " + key);
        }
    }

    private void setReadButtonState(int state) {
        // Set the state of the read button based on the received value
        if (state == 1) {
            // Enable the read button
            readToggle.setText("Read On");
            readToggle.setBackgroundColor(Color.GREEN);
        } else {
            // Disable the read button
            readToggle.setText("Read Off");
            readToggle.setBackgroundColor(Color.RED);
        }
    }

    private void setWriteButtonState(int state) {
        if (state == 1) {
            // Enable the read button
            writeToggle.setText("Write On");
            writeToggle.setBackgroundColor(Color.GREEN);
        } else {
            // Disable the read button
            writeToggle.setText("Write Off");
            writeToggle.setBackgroundColor(Color.RED);
        }
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case REQUEST_CONNECT_DEVICE_SECURE:
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data, true);
                }
                break;
            case REQUEST_CONNECT_DEVICE_INSECURE:
                // When DeviceListActivity returns with a device to connect
                if (resultCode == Activity.RESULT_OK) {
                    connectDevice(data, false);
                }
                break;
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    // Bluetooth is now enabled, so set up a chat session
                    setupChat();
                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "BT not enabled");
                    FragmentActivity activity = getActivity();
                    if (activity != null) {
                        Toast.makeText(activity, R.string.bt_not_enabled_leaving,
                                Toast.LENGTH_SHORT).show();
                        activity.finish();
                    }
                }
        }
    }

//09.10
    /*
private void updateUserList(String userListMessage) {
    // Assuming userListPopup is a TextView in your layout
    TextView userListPopup = getView().findViewById(R.id.userListTextView);

    // Set the new user list message
    userListPopup.setText(userListMessage);

    // Set movement method to make the TextView scrollable
    userListPopup.setMovementMethod(new ScrollingMovementMethod());

    // Create a PopupWindow
    PopupWindow popupWindow = new PopupWindow(userListPopup, ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT, true);
    popupWindow.setBackgroundDrawable(new ColorDrawable(Color.WHITE));

    // Show the PopupWindow
    popupWindow.showAtLocation(requireView(), Gravity.CENTER, 0, 0);
}*/

    /**
     * Establish connection with other device
     *
     * @param data   An {@link Intent} with {@link DeviceListActivity#EXTRA_DEVICE_ADDRESS} extra.
     * @param secure Socket Security type - Secure (true) , Insecure (false)
     */
    private void connectDevice(Intent data, boolean secure) {
        // Get the device MAC address
        Bundle extras = data.getExtras();
        if (extras == null) {
            return;
        }
        String address = extras.getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
        // Get the BluetoothDevice object
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        // Attempt to connect to the device
        mChatService.connect(device, secure);

        showPasswordDialog();
    }

    private void showPasswordDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle("Enter Password");

        final EditText input = new EditText(getActivity());
        input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
        builder.setView(input);

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {

                //GENERATING RSA KEYS and sending them over to the stm in format of {e1:131073e2:m1:401025m2:1639}000000000000000000

                        rsaGenKeys(pub, priv);
                        String publicKeyString = formatPublicKey(pub);
                        String originalMessage = publicKeyString;
                        int spacesToAdd = 500 - originalMessage.length();
                        StringBuilder paddedMessage = new StringBuilder(originalMessage);
                        for (int i = 0; i < spacesToAdd; i++) {
                            paddedMessage.append(" ");
                        }
                        sendMessage(paddedMessage.toString());



                Handler handler2 = new Handler();
                handler2.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                String password = input.getText().toString();
                // Send the entered password to the remote device
                /*String originalMessage = "{password:" + password + "}";
                int spacesToAdd = 500 - originalMessage.length();
                StringBuilder paddedMessage = new StringBuilder(originalMessage);
                for (int i = 0; i < spacesToAdd; i++) {
                    paddedMessage.append(" ");
                }
                sendMessage(paddedMessage.toString());*/

                String messageToEncrypt = "pass:" + password + "}";
                encryptAndSendMessage(messageToEncrypt);
                    }
                }, 2000); // 500 milliseconds (half a second)
            }
        });

        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                getActivity().finish();
            }
        });

        builder.show();
    }

    @Override
    public void onCreateOptionsMenu(@NonNull Menu menu, MenuInflater inflater) {
        inflater.inflate(R.menu.bluetooth_chat, menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.secure_connect_scan: {
                // Launch the DeviceListActivity to see devices and do scan
                Intent serverIntent = new Intent(getActivity(), DeviceListActivity.class);
                startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE_SECURE);
                return true;
            }
            case R.id.discoverable: {
                // Ensure this device is discoverable by others
                ensureDiscoverable();
                return true;
            }
        }
        return false;
    }

}
