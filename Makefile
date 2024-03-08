REPORT_DIR=report

pdf:
	mkdir -p $(REPORT_DIR)/out
	pdflatex --output-directory $(REPORT_DIR)/out $(REPORT_DIR)/report.tex

clean:
	rm -rf $(REPORT_DIR)/out
