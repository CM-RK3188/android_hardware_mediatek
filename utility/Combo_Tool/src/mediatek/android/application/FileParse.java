package mediatek.android.application;

import java.io.*;
import java.util.*;

import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

public class FileParse {
	
	private List<Section> sectList = new ArrayList<Section>();
	private String ConfPath = null;
	String TAG="FileParse";
	/**
	 * read all section from file
	 * @param fileName        witch file will to read 
	 * @return List<Section>  contain all section of configure file
	 * @throws IOException 
	 * @throws IOException
	 */
	
	public FileParse(String FileName) 
	{
		try
		{	/*
			if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState()))
			{				
				ConfPath = Environment.getExternalStorageDirectory().getPath() + "/" + FileName;				
			
			}
			else
			*/
			{
				Log.w(TAG, "Can't find SD Card! Put the init file to /data");
				ConfPath = "/data/" + FileName;
			}	
			Log.d(TAG, "configture file = " + ConfPath);
			readFromFile(ConfPath);
		}
		catch (FileNotFoundException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
			//throws IOException;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void initConfFile(String ConfFile) throws IOException
	{
		File targetFile = new File(ConfFile);

		OutputStreamWriter osw;
		try {
			osw = new OutputStreamWriter(new FileOutputStream(targetFile, false));
			osw.write("[simple-wifi]" + "\n");
			osw.write("SSID = Mediatek" + "\n");
			osw.write("PASSWORD = 12345678" + "\n");
			osw.write("RSSI_THRESHOLD = -70" + "\n");
			osw.write("\n");
			
			osw.write("[advance-wifi]" + "\n");
			osw.write("RX_CHANNEL = Channel 1 [2412MHz]" + "\n");
			osw.write("RX_BANDWIDTH = 20MHz" + "\n");
			
			osw.write("TX_CHANNEL = Channel 1 [2412MHz]" + "\n");
			osw.write("TX_PACKET_LENGTH = 1024" + "\n");
			osw.write("TX_PACKET_COUNT = 3000" + "\n");
			osw.write("TX_POWER = 18" + "\n");
			osw.write("TX_RATE = 1M" + "\n");
			osw.write("TX_PREAMBLE = Normal" + "\n");
			osw.write("TX_MODE = continuous packet tx" + "\n");
			osw.write("TX_GUARD_INTERVAL = 800ns" + "\n");
			osw.write("TX_BANDWIDTH = 20MHz" + "\n");
			osw.write("TX_FREQOFFSET = 30" + "\n");
			
			osw.write("\n");
			
			osw.write("[advance-bt]" + "\n");
			osw.write("RX_PATTERN = 0000" + "\n");
			osw.write("RX_PACKET_TYPE = DM1(0x03)" + "\n");
			osw.write("RX_FREQUENCY = 60" + "\n");
			osw.write("RX_TESTER_ADDRESS = A5F0C3" + "\n");
			
			osw.write("TX_PATTERN = 0000" + "\n");
			osw.write("TX_CHANNEL = Single Frequency" + "\n");
			osw.write("TX_FREQUENCY = 60" + "\n");
			osw.write("TX_PACKET_TYPE = NULL(0x00)" + "\n");
			osw.write("TX_PACKET_LENGTH = 200" + "\n");
			osw.write("\n");
			
			osw.flush();
			osw.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
	}
	
	private void readFromFile(String ConfFile) throws IOException
	{
		File SourceFile = new File(ConfFile);
		FileInputStream Fin = null;
		Section tmpSection = null;
		
		try {
			if (!SourceFile.exists())
			{
				SourceFile.createNewFile();
				initConfFile(ConfFile);
			}

			Fin = new FileInputStream(SourceFile);

		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 

		InputStream in = new BufferedInputStream(Fin);
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		
		String Line;
		
		while ((Line = br.readLine()) != null)
		{
			//´¦Àí¿ÕÐÐ
			if (Line.equals(""))
			{
				continue;
			}
			
			if (Line.startsWith("[") && Line.endsWith("]"))
			{		
				tmpSection = new Section(Line);
				sectList.add(tmpSection);
				continue;
			}
			
			//if (null != SectionArr[iIndex])
			if (null != tmpSection)
			{
				tmpSection.setSectionKeyValue(Line);
			}
		}
		
		System.out.println(sectList.size());
	}
	
	/**
	 * update section context
	 * @param TargeSection which section need to update
	 * @param Key          key name
	 * @param Value        key vale
	 * @return boolean     
	 * 		   true        success
	 * 		   false       fail 
	 * @throws IOException
	 */
	/*
	public boolean updateSection(Section TargeSection, String Key, String Value) throws IOException
	{
		boolean result = true;
		String Context;
		
		return result;
	}
	*/
	private Section getSection(String sectionName)
	{
		Section result = null;
		for (int i = 0; i < sectList.size(); i ++)
		{
			result = sectList.get(i);
			if (result.getSectionName().equals(sectionName))
			{
				return result;
			}
		}
		
		return result;
	}
	
	public void updateKeyValue(String sectionName, String Key, String Value)
	{
		Section tagSection = getSection(sectionName);
		
		tagSection.updateSectionValueByKey(Key, Value);
	}
	
	public String getValueByKey(String sectionName, String Key)
	{
		String result = null;
		Section tagSection = getSection(sectionName);
		result = tagSection.getValueByKey(Key);
		return result;
	}
	
	public void addKeyValue(String sectionName, String Key, String Value)
	{
		Section tagSection = getSection(sectionName);
		tagSection.addSectionKeyValue(Key, Value);
	}
	
	public void delKeyValue(String sectionName, String Key)
	{
		Section tagSection = getSection(sectionName);
		tagSection.delSectionKeyValue(Key);
	}
	
	/**
	 * write all section to file
	 * @param fileName     witch file will to write 
	 * @param sectList     all section list structure
	 * @return 
	 * @return boolean     
	 * 		   true        success
	 * 		   false       fail 
	 * @throws IOException 
	 * @throws IOException
	 */
	public void updateConFile(String FileName) throws IOException
	{
		writeToFile(ConfPath);
	}

	private  boolean writeToFile(String fileName) throws IOException
	{
		boolean bResult = true;
		
		File targetFile = new File(fileName);
		List<String> KeyValueList = null;
		
		OutputStreamWriter osw;
		try {
			osw = new OutputStreamWriter(new FileOutputStream(targetFile, false));
			for (int i = 0; i < sectList.size(); i ++)
			{
				Section tmpSection = (Section)sectList.get(i);
				osw.write(tmpSection.getSectionName()+ "\n");
				KeyValueList = tmpSection.getSectionKeyValue();
				for (int j = 0; j < KeyValueList.size(); j ++)
				{
					osw.write(KeyValueList.get(j) + "\n");
					System.out.println(KeyValueList.get(j));
				}
				
				osw.write("\n");
			}
			osw.flush();
			osw.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		return bResult;
	}

	class Section
	{
		private String sectionName = "";
//		private String sectionContext = "";
		private List<String> keyValueList = new ArrayList<String>();
		
		public Section(String name)
		{
			this.sectionName = name;
		}
		
		public String getSectionName()
		{
			return this.sectionName;
		}
		
		public String getValueByKey(String Key)
		{
			String Value = "";
			String tmpString = "";
			int iStart = 0;
			int iLength = 0;
			
			for (int i = 0; i < keyValueList.size(); i ++)
			{
				tmpString = keyValueList.get(i);
				if (tmpString.startsWith(Key))
				{
					iStart = tmpString.indexOf("=");
					iLength = tmpString.length();
					Value = tmpString.substring(iStart + 1, iLength);
					Value = Value.trim();
				}
			}
			
			return Value;
		}
		
		public List<String> getSectionKeyValue()
		{
			return this.keyValueList;
		}
		
		public void setSectionKeyValue(String keyValue)
		{
			this.keyValueList.add(keyValue);
		}
		
		public void addSectionKeyValue(String Key, String Value)
		{
			String newKeyValue = "";
			
			newKeyValue = Key + " = " + Value;
			
			this.keyValueList.add(newKeyValue);
		}

		public boolean delSectionKeyValue(String Key)
		{
			boolean result = true;

			String tmpString = "";

			
			for (int i = 0; i < keyValueList.size(); i ++)
			{
				tmpString = keyValueList.get(i);
				if (tmpString.startsWith(Key))
				{
					this.keyValueList.remove(i);
				}
			}
			
			return result;
		}
		
		public boolean updateSectionValueByKey(String Key, String Value)
		{
			boolean result = true;
			String tmpString = "";
			String NewString = Key + " = " + Value;

			
			for (int i = 0; i < keyValueList.size(); i ++)
			{
				tmpString = keyValueList.get(i);
				if (tmpString.startsWith(Key))
				{
					this.keyValueList.remove(i);
					this.keyValueList.add(i, NewString);
				}
			}
			
			return result;
		}
	}
}
