namespace LUR_Packet_Scout
{
    partial class Form1
    {
        /// <summary>
        /// Required.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// All resources are open source.
        /// </summary>
        /// <param name="disposing">Set to true if values should be disposed of.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code by Windows Form Designer

        /// <summary>
        /// Do not modify.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.ndoor = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.btnstart = new System.Windows.Forms.Button();
            this.rblog = new System.Windows.Forms.RichTextBox();
            this.bgw = new System.ComponentModel.BackgroundWorker();
            this.tmbuf = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.ndoor)).BeginInit();
            this.SuspendLayout();
            // 
            // ndoor
            // 
            this.ndoor.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ndoor.Location = new System.Drawing.Point(54, 9);
            this.ndoor.Maximum = new decimal(new int[] {
            9000000,
            0,
            0,
            0});
            this.ndoor.Name = "ndoor";
            this.ndoor.Size = new System.Drawing.Size(83, 22);
            this.ndoor.TabIndex = 0;
            this.ndoor.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.ndoor.Value = new decimal(new int[] {
            1001,
            0,
            0,
            0});
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 11);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(36, 16);
            this.label1.TabIndex = 1;
            this.label1.Text = "Port";
            // 
            // btnstart
            // 
            this.btnstart.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnstart.Location = new System.Drawing.Point(175, 9);
            this.btnstart.Name = "btnstart";
            this.btnstart.Size = new System.Drawing.Size(75, 23);
            this.btnstart.TabIndex = 2;
            this.btnstart.Text = "Start";
            this.btnstart.UseVisualStyleBackColor = true;
            this.btnstart.Click += new System.EventHandler(this.btnstart_Click);
            // 
            // rblog
            // 
            this.rblog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.rblog.BackColor = System.Drawing.Color.White;
            this.rblog.Location = new System.Drawing.Point(12, 37);
            this.rblog.Name = "rblog";
            this.rblog.ReadOnly = true;
            this.rblog.Size = new System.Drawing.Size(352, 178);
            this.rblog.TabIndex = 3;
            this.rblog.Text = "";
            // 
            // bgw
            // 
            this.bgw.WorkerSupportsCancellation = true;
            this.bgw.DoWork += new System.ComponentModel.DoWorkEventHandler(this.bgw_DoWork);
            // 
            // tmbuf
            // 
            this.tmbuf.Interval = 1000;
            this.tmbuf.Tick += new System.EventHandler(this.tmbuf_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(376, 227);
            this.Controls.Add(this.rblog);
            this.Controls.Add(this.btnstart);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ndoor);
            this.Name = "Form1";
            this.Text = "LEGO Universe Rebuilt Packet Scout";
            ((System.ComponentModel.ISupportInitialize)(this.ndoor)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.NumericUpDown ndoor;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnstart;
        private System.Windows.Forms.RichTextBox rblog;
        private System.ComponentModel.BackgroundWorker bgw;
        private System.Windows.Forms.Timer tmbuf;
    }
}

