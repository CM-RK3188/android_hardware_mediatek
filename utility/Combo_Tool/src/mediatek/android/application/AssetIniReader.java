package mediatek.android.application;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.util.HashMap;
import java.util.Properties;

import android.content.Context;

/**
 * ���ڶ�ȡini�����ļ�
 * 
 * @author USER
 * 
 */
public class AssetIniReader {

	// ���ڴ�������ļ�������ֵ
	protected HashMap<String, Properties> sections = new HashMap<String, Properties>();
	private transient String currtionSecion;
	private transient Properties current;

	/**
	 * ��ȡ�ļ�
	 * 
	 * @param filename
	 *            �ļ���
	 * @throws IOException
	 */
	public static String[] readAssetTxt(Context context, String fileName) {
		try {
			InputStreamReader inputReader = new InputStreamReader(context

			.getResources().getAssets().open(fileName));

			BufferedReader bufReader = new BufferedReader(inputReader);

			LineNumberReader reader = new LineNumberReader(bufReader);

			String s = reader.readLine();

			String[] result = new String[10];

			int lines = 0;
			while (s != null) {
				result[lines] = s;
				lines++;
				s = reader.readLine();

				// Log.i(TAG, "lines" + lines + ":" + s);

			}

			reader.close();

			bufReader.close();

			return result;

		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	public AssetIniReader(String name, Context context) throws IOException {
		InputStream in = context.getAssets().open(name);
		InputStreamReader reader = new InputStreamReader(in, "GBK");
		BufferedReader read = null;
		try {
			if (reader != null) {
				read = new BufferedReader(reader);
				reader(read);
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new FileNotFoundException("�ļ������ڻ����ļ���ȡʧ��");
		}
	}

	/**
	 * ����ÿ�ζ�ȡ�ļ�һ��
	 * 
	 * @param reader
	 *            �ļ���
	 * @throws IOException
	 */
	private void reader(BufferedReader reader) throws IOException {
		// TODO Auto-generated method stub
		String line = null;
		try {
			while ((line = reader.readLine()) != null) {
				parseLine(line);
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			throw new IOException("�ļ����ݶ�ȡʧ��");
		}
	}

	/**
	 * ��ȡini�ļ�������ֵ
	 * 
	 * @param line
	 *            ini�ļ�ÿ������
	 */
	private void parseLine(String line) {
		// TODO Auto-generated method stub
		try {
			if (line != null) {
				line = line.trim();
				if (line.matches("\\[.*\\]")) {
					currtionSecion = line.replaceFirst("\\[(.*)\\]", "$1");
					current = new Properties();
					sections.put(currtionSecion, current);
				} else if (line.matches(".*=.*")) {
					if (current != null) {
						int i = line.indexOf('=');
						int iLength = 0;
						iLength = line.length();
						String name = line.substring(0, i - 1);

						String value = line.substring(i + 2, iLength);

						current.setProperty(name, value);

					}
				}
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	/**
	 * ���ڻ�ȡ����ֵ��ֵ
	 * 
	 * @param section
	 *            �������Ե�ֵ
	 * @param name
	 *            ����ֵ����
	 * @return ����ֵ��ֵ
	 */
	public String getValue(String section, String name) {

		Properties p = (Properties) sections.get(section);

		if (p == null) {
			return null;
		}
		String value = p.getProperty(name);

		return value;
	}
}
