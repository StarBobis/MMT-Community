﻿using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NMBT_GUI
{
    partial class ConfigMod
    {

        public void resetConfig()
        {
            //Merge配置
            comboBoxGameType.SelectedIndex = -1;

            checkBox_ForbidAutoTexture.Checked = true;
            checkBox_ForbidAutoTexture.Checked = false;

            //Split配置
            textBoxColorRGBR.Text = string.Empty;
            checkBoxColorRGBR.Checked = true;
            checkBoxColorRGBR.Checked = false;

            textBoxColorRGBG.Text = string.Empty;
            checkBoxColorRGBG.Checked = true;
            checkBoxColorRGBG.Checked = false;

            textBoxColorRGBB.Text = string.Empty;
            checkBoxColorRGBB.Checked = true;
            checkBoxColorRGBB.Checked = false;

            textBoxColorRGBA.Text = string.Empty;
            checkBoxColorRGBA.Checked = true;
            checkBoxColorRGBA.Checked = false;

            checkBoxTANGENT.Checked = true;
            checkBoxTANGENT.Checked = false;
            comboBoxTANGENT.SelectedIndex = -1;

            checkBoxColorRecalculate.Checked = true;
            checkBoxColorRecalculate.Checked = false;
            comboBoxCOLORAlgorithm.SelectedIndex = -1;


            //Normal和Tangent的Reverse配置
            checkBoxNormalReverseX.Checked = true;
            checkBoxNormalReverseX.Checked = false;
            checkBoxNormalReverseY.Checked = true;
            checkBoxNormalReverseY.Checked = false;
            checkBoxNormalReverseZ.Checked = true;
            checkBoxNormalReverseZ.Checked = false;

            checkBoxTangentReverseX.Checked = true;
            checkBoxTangentReverseX.Checked = false;
            checkBoxTangentReverseY.Checked = true;
            checkBoxTangentReverseY.Checked = false;
            checkBoxTangentReverseZ.Checked = true;
            checkBoxTangentReverseZ.Checked = false;
            checkBoxTangentReverseW.Checked = true;
            checkBoxTangentReverseW.Checked = false;

            //默认不使用hash自动贴图
            checkBoxUseHashTexture.Checked = true;
            checkBoxUseHashTexture.Checked = false;

            //除非当前游戏是HI3或HSR，则默认使用基于Hash的自动贴图
            if (this.CurrentGame == "HI3" || this.CurrentGame == "HSR")
            {
                checkBoxUseHashTexture.Checked = true;
            }

        }

        public void readConfig()
        {
            if (File.Exists(Path_Game_ConfigJson))
            {
                string jsonData = File.ReadAllText(Path_Game_ConfigJson);
                JArray jsonArray = JArray.Parse(jsonData);

                foreach (JObject obj in jsonArray)
                {
                    string drawIB = (string)obj["DrawIB"];
                    if (this.DrawIB != drawIB)
                    {
                        continue;
                    }


                    //读取Merge配置
                    string gameType = (string)obj["GameType"];
                    string forcePointlistIndex = (string)obj["ForcePointlistIndex"];
                    string forceTrianglelistIndex = (string)obj["ForceTrianglelistIndex"];
                    comboBoxGameType.SelectedItem = gameType;
                   

                    //读取Split配置
                    JObject colorDict = (JObject)obj["Color"];
                    string rgb_r = (string)colorDict["rgb_r"];
                    string rgb_g = (string)colorDict["rgb_g"];
                    string rgb_b = (string)colorDict["rgb_b"];
                    string rgb_a = (string)colorDict["rgb_a"];
                    if (rgb_r != "default")
                    {
                        checkBoxColorRGBR.Checked = true;
                        textBoxColorRGBR.Text = rgb_r;
                    }
                    if (rgb_g != "default")
                    {
                        checkBoxColorRGBG.Checked = true;
                        textBoxColorRGBG.Text = rgb_g;
                    }
                    if (rgb_b != "default")
                    {
                        checkBoxColorRGBB.Checked = true;
                        textBoxColorRGBB.Text = rgb_b;
                    }
                    if (rgb_a != "default")
                    {
                        checkBoxColorRGBA.Checked = true;
                        textBoxColorRGBA.Text = rgb_a;
                    }
                    string tangentAlgorithm = (string)obj["TangentAlgorithm"];
                    if (tangentAlgorithm != "")
                    {
                        checkBoxTANGENT.Checked = true;
                        comboBoxTANGENT.SelectedItem = tangentAlgorithm;
                    }
                    else
                    {
                        checkBoxTANGENT.Checked = false;
                        comboBoxTANGENT.SelectedIndex = -1;
                    }
                   
                    string colorAlgorithm = (string)obj["ColorAlgorithm"];
                    if (colorAlgorithm != "")
                    {
                        checkBoxColorRecalculate.Checked = true;
                        comboBoxCOLORAlgorithm.SelectedItem = colorAlgorithm;
                    }
                    

                    //读取NORMAL和TANGENT reverse配置
                    bool NormalReverseX = (bool)obj["NormalReverseX"];
                    bool NormalReverseY = (bool)obj["NormalReverseY"];
                    bool NormalReverseZ = (bool)obj["NormalReverseZ"];
                    checkBoxNormalReverseX.Checked = NormalReverseX;
                    checkBoxNormalReverseY.Checked = NormalReverseY;
                    checkBoxNormalReverseZ.Checked = NormalReverseZ;

                    bool TangentReverseX = (bool)obj["TangentReverseX"];
                    bool TangentReverseY = (bool)obj["TangentReverseY"];
                    bool TangentReverseZ = (bool)obj["TangentReverseZ"];
                    bool TangentReverseW = (bool)obj["TangentReverseW"];
                    checkBoxTangentReverseX.Checked = TangentReverseX;
                    checkBoxTangentReverseY.Checked = TangentReverseY;
                    checkBoxTangentReverseZ.Checked = TangentReverseZ;
                    checkBoxTangentReverseW.Checked = TangentReverseW;

                    //只有包含此值时才赋值，用于兼容旧版本没有这个值的
                    if (obj.ContainsKey("ForbidAutoTexture"))
                    {
                        checkBox_ForbidAutoTexture.Checked = (bool)obj["ForbidAutoTexture"];
                    }

                    //只有包含此值时才赋值，用于兼容旧版本没有这个值的
                    if (obj.ContainsKey("UseHashTexture"))
                    {
                        checkBoxUseHashTexture.Checked = (bool)obj["UseHashTexture"];
                    }


                }
            }

        }

        public void saveConfig()
        {
            // 创建一个空的JObject对象，用来保存配置
            JArray jsonArray = new JArray();
            if (File.Exists(Path_Game_ConfigJson))
            {
                // 文件存在，读取文件内容
                string jsonData = File.ReadAllText(Path_Game_ConfigJson);
                jsonArray = JArray.Parse(jsonData);
            }

            //1.查找是否存在该DrawIB，如果存在，就进行修改，否则新建一个添加到Jarray里，然后保存。
            bool findDrawIB = false;
            for (int i = 0; i < jsonArray.Count; i++)
            {
                // 使用[i]获取并修改JArray里的东西会影响到 jsonArray 中的元素，而使用foreach不会。
                JObject obj = (JObject)jsonArray[i];
                string drawIB = (string)obj["DrawIB"];
                if (drawIB  == DrawIB)
                {
                    findDrawIB = true;
                    break;
                }
            }

            if (findDrawIB)
            {
                //如果存在该DrawIB，那就修改这个DrawIB的值
                for (int i = 0; i < jsonArray.Count; i++)
                {
                    JObject obj = (JObject)jsonArray[i];
                    string drawIB = (string)obj["DrawIB"];
                    if (drawIB != DrawIB)
                    {
                        continue;
                    }
                    
                    saveMergeConfig(obj);
                    saveSplitConfig(obj);
                    saveNormalTangentReverseConfig(obj);

                }
            }
            else
            {
                //如果没找到，那就新建一个JObject来装载各个属性，然后放到这个JArray里
                JObject obj = new JObject();
                obj["DrawIB"] = DrawIB;
                saveMergeConfig(obj);
                saveSplitConfig(obj);
                saveNormalTangentReverseConfig(obj);
                jsonArray.Add(obj);
            }

            // 将JObject转换为JSON字符串
            string json_string = jsonArray.ToString(Formatting.Indented);

            // 将JSON字符串写入文件
            File.WriteAllText(Path_Game_ConfigJson, json_string);


        }
        void saveNormalTangentReverseConfig(JObject obj)
        {
            obj["NormalReverseX"] = checkBoxNormalReverseX.Checked;
            obj["NormalReverseY"] = checkBoxNormalReverseY.Checked;
            obj["NormalReverseZ"] = checkBoxNormalReverseZ.Checked;

            obj["TangentReverseX"] = checkBoxTangentReverseX.Checked;
            obj["TangentReverseY"] = checkBoxTangentReverseY.Checked;
            obj["TangentReverseZ"] = checkBoxTangentReverseZ.Checked;
            obj["TangentReverseW"] = checkBoxTangentReverseW.Checked;
        }

        //保存Merge配置
        public void saveMergeConfig(JObject obj)
        {
            //C#的参数是传递引用，所以直接修改会影响传递过来的obj
            obj["GameType"] = comboBoxGameType.Text;
        }

        public void saveSplitConfig(JObject obj)
        {
            JObject colorObject = new JObject();
            if (checkBoxColorRGBR.Checked)
            {
                colorObject["rgb_r"] = textBoxColorRGBR.Text; 
            }
            else
            {
                colorObject["rgb_r"] = "default";
            }
            if (checkBoxColorRGBG.Checked)
            {
                colorObject["rgb_g"] = textBoxColorRGBG.Text;
            }
            else
            {
                colorObject["rgb_g"] = "default";
            }
            if (checkBoxColorRGBB.Checked)
            {
                colorObject["rgb_b"] = textBoxColorRGBB.Text;
            }
            else
            {
                colorObject["rgb_b"] = "default";
            }
            if (checkBoxColorRGBA.Checked)
            {
                colorObject["rgb_a"] = textBoxColorRGBA.Text;
            }
            else
            {
                colorObject["rgb_a"] = "default";
            }
            obj["Color"] = colorObject;

            
            if (checkBoxTANGENT.Checked)
            {
                obj["TangentAlgorithm"] = comboBoxTANGENT.Text;
            }
            else
            {
                obj["TangentAlgorithm"] = "";
            }
            if (checkBoxColorRecalculate.Checked)
            {
                obj["ColorAlgorithm"] = comboBoxCOLORAlgorithm.Text;
            }
            else
            {
                obj["ColorAlgorithm"] = "";
            }

            obj["ForbidAutoTexture"] = checkBox_ForbidAutoTexture.Checked;
            obj["UseHashTexture"] = checkBoxUseHashTexture.Checked;
        }
      

        public bool ContainsChinese(string input)
        {
            // 使用正则表达式匹配中文字符
            Regex regex = new Regex(@"[\u4e00-\u9fa5]");
            return regex.IsMatch(input);
        }

        public bool HashFormatCheck(string name, string value)
        {
            if (!StringSanityCheck(name, value))
            {
                return false;
            }
            else if (value.Contains(" "))
            {
                ShowMessageBox(name + " can't contains Space character", name + "不能出现空格");
                //MessageBox.Show(name + "不能出现空格", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            else if (value.Length != 8)
            {
                ShowMessageBox(name + " is a hash value so length must be 8", name + "是Hash值，长度必须为8");
                //MessageBox.Show(name + "是Hash值，长度必须为8", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            return true;
        }

        public bool StringSanityCheck(string name, string value)
        {
            if (string.IsNullOrEmpty(value))
            {
                ShowMessageBox(name + " can't be empty", name + "不能为空");
                //MessageBox.Show(name + "不能为空", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }
            else if (ContainsChinese(value))
            {
                ShowMessageBox(name + " can't contains Chinese", name + "不能出现中文");
                //MessageBox.Show(name + "不能出现中文", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return false;
            }

            return true;
        }
        public bool IsInRange(string input)
        {
            if (int.TryParse(input, out int number))
            {
                // 检查数字是否在0到255之间
                if (number >= 0 && number <= 255)
                {
                    return true;
                }
            }

            return false;
        }

        public bool checkConfig()
        {
            if (checkBoxColorRGBR.Checked)
            {
                string rgbr = textBoxColorRGBR.Text;
                if (!IsInRange(rgbr))
                {
                    ShowMessageBox("Color rgb_r must between 0 to 255","Color值rgb_r必须位于0-255之间");
                    //MessageBox.Show("Color值rgb_r必须位于0-255之间", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return false;
                }
            }
            if (checkBoxColorRGBG.Checked)
            {
                string rgbg = textBoxColorRGBG.Text;
                if (!IsInRange(rgbg))
                {
                    ShowMessageBox("Color rgb_g must between 0 to 255", "Color值rgb_g必须位于0-255之间");

                    //MessageBox.Show("Color值rgb_g必须位于0-255之间", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return false;
                }
            }
            if (checkBoxColorRGBB.Checked)
            {
                string rgbb = textBoxColorRGBB.Text;
                if (!IsInRange(rgbb))
                {
                    ShowMessageBox("Color rgb_b must between 0 to 255", "Color值rgb_b必须位于0-255之间");

                    //MessageBox.Show("Color值rgb_b必须位于0-255之间", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return false;
                }
            }
            if (checkBoxColorRGBA.Checked)
            {
                string rgba = textBoxColorRGBA.Text;
                if (!IsInRange(rgba))
                {

                    ShowMessageBox("Color rgb_a must between 0 to 255", "Color值rgb_a必须位于0-255之间");

                    //MessageBox.Show("Color值rgb_a必须位于0-255之间", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                    return false;
                }
            }

            return true;
        }


        

    }
}
