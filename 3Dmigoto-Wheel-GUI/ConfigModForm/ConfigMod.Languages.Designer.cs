﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NMBT_GUI
{
    partial class ConfigMod
    {
        void ShowMessageBox(string EnglishTip, string ChineseTip)
        {
            if (currentLanguage == "en")
            {
                MessageBox.Show(EnglishTip);
            }
            else if (currentLanguage == "zh-cn")
            {
                MessageBox.Show(ChineseTip);
            }
        }

        void switchToEnglish()
        {

        }

        void switchToChinese()
        {
            Menu_InitializeConfig_ToolStripMenuItem.Text = "初始化配置";
            Menu_COLORPresetToolStripMenuItem.Text = "COLOR属性预设";
            groupBoxExtractConfig.Text = "提取配置";
            labelExtractType.Text = "提取类型";

            groupBoxModGenerateConfig.Text = "二创模型生成配置";
            groupBoxCOLORSET.Text = "特殊分量值配置";
            checkBoxColorRGBR.Text = "红通道";
            checkBoxColorRGBG.Text = "绿通道";
            checkBoxColorRGBB.Text = "蓝通道";
            checkBoxColorRGBA.Text = "Aplha通道(轮廓线)";

            checkBoxColorRecalculate.Text = "重新计算COLOR值";

            groupBox_NORMAL_FLIP.Text = "NORMAL分量翻转";
            groupBox_TANGENT_FLIP.Text = "TANGENT分量翻转";

            checkBoxTANGENT.Text = "重新计算TANGENT值";

            checkBox_ForbidAutoTexture.Text = "禁止全自动ini贴图生成";
            checkBoxUseHashTexture.Text = "使用Hash风格替换贴图";
        }

    }
}
