void manageRFID()
{
  if (mfrc522.PICC_IsNewCardPresent())
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Detected!");
    lcd.setCursor(0, 1);
    lcd.print("Please Wait.....");
    if (mfrc522.PICC_ReadCardSerial())
    {
      if (readCard())
      {
        success(500);
        if (readByte[15] != 0) // && readByte[0] == 107) // verify the card and available quantity on card
        {
          dumpToWriteVar(readByte, 16);
          if (!writeCard(writeByte))
          {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Failed!");
            lcd.setCursor(0, 1);
            lcd.print("Try Again");
            warning();
          }
          else
          {
            byte lastData = readByte[15];
            readCard();
            if (getStock() != 0) // check the stock before proceeding
            {
              if (lastData != readByte[15]) // check the stock before proceeding
              {
                delay(500);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Left for Card:");
                lcd.setCursor(0, 1);
                lcd.print(writeByte[15]);
                delay(3000);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Receive Pad");
                lcd.setCursor(0, 1);
                lcd.print("Thank you!");                
                runMotor();
                success(800);
                delay(1000);
              }
              else // print if card scan time is fast
              {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Sorry");
                lcd.setCursor(0, 1);
                lcd.print("Hold card for 1Sec");
                warning();
              }
            }

            else // print if no stock remaining
            {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Sorry");
              lcd.setCursor(0, 1);
              lcd.print("No Stocks");
              warning();
            }
          }
        }
        else
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Opps!");
          lcd.setCursor(0, 1);
          lcd.print("Zero Balance");
          warning();
        }
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Failed!");
        lcd.setCursor(0, 1);
        lcd.print("Try Again");
        warning();
      }
    }
    halt();
    status = 'n';
    changeDone = true;
  }
}


bool readCard()
{
  byte buffersize = 18;
  if (auth_A())
  {
    rfidstatus = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, readByte, &buffersize);
    if (rfidstatus != MFRC522::STATUS_OK)
    {
      return false;
    }
  }
  return true;
}
bool writeCard(byte writeByte[])
{
  if (auth_B())
  {
    rfidstatus = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, writeByte, 16);
    if (rfidstatus != MFRC522::STATUS_OK)
    {
      return false;
    }
  }
  return true;
}

void dumpToWriteVar(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i]);
    writeByte[i] = buffer[i];
  }
  writeByte[15]--;
}
void halt()
{
  mfrc522.PICC_HaltA();      // Halt PICC
  mfrc522.PCD_StopCrypto1(); // Stop encryption on PCD
}
bool auth_A()
{
  rfidstatus = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, authAddr, &key, &(mfrc522.uid));
  if (rfidstatus != MFRC522::STATUS_OK)
  {
    return false;
  }
  return true;
}
bool auth_B()
{
  rfidstatus = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, authAddr, &key, &(mfrc522.uid));
  if (rfidstatus != MFRC522::STATUS_OK)
  {
    return false;
  }
  return true;
}