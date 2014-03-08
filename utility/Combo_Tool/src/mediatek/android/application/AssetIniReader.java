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
 * 用于读取ini配置文件
 * 
 * @author USER
 * 
 */
public class AssetIniReader {

	// 用于存放配置文件的属性值
	protected HashMap<String, Properties> sections = new HashMap<String, Properties>();
	private transient String currtionSecion;
	private transient Properties current;

	/**
	 * 读取文件
	 * 
	 * @param filename
	 *            文件名
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
			throw new FileNotFoundException("文件不存在或者文件读取失败");
		}
	}

	/**
	 * 设置每次读取文件一行
	 * 
	 * @param reader
	 *            文件流
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
			throw new IOException("文件内容读取失败");
		}
	}

	/**
	 * 获取ini文件的属性值
	 * 
	 * @param line
	 *            ini文件每行数据
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
	 * 用于获取属性值的值
	 * 
	 * @param section
	 *            整体属性的值
	 * @param name
	 *            属性值名字
	 * @return 属性值的值
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
