#!/usr/bin/env python3

import os
import sys
import urllib.parse

def parse_parameters(raw_str):
	parameters = {}
	if raw_str:
		pairs = raw_str.split('&')
		for pair in pairs:
			if '=' in pair:
				name, value = pair.split('=', 1)
				parameters[urllib.parse.unquote_plus(name)] = urllib.parse.unquote_plus(value)
	return parameters

def main():
	request_method = os.environ.get('REQUEST_METHOD', '')
	query_string = os.environ.get('QUERY_STRING', '')
	content_length_str = os.environ.get('CONTENT_LENGTH', '')
	content_length = int(content_length_str) if content_length_str else 0

	query_params = parse_parameters(query_string)

	print("Content-Type: text/html\r\n\r\n")
	print("<html><body>")
	print('<p><a href="/">Volver</a></p>')

	if request_method == "POST" and content_length > 0:
		body = sys.stdin.read(content_length)
		body_params = parse_parameters(body)

		modified_body = body.replace('0', '2')

		print("<h3>POST Body (modified '0' → '2'):</h3>")
		print(f"<pre>{modified_body}</pre>")

		print("<h3>Parsed POST Parameters:</h3><ul>")
		for name, value in body_params.items():
			print(f"<li>{name} = {value}</li>")
		print("</ul>")
	else:
		print("<h3>GET Parameters:</h3><ul>")
		for name, value in query_params.items():
			print(f"<li>{name} = {value}</li>")
		print("</ul>")

	print("</body></html>")

if __name__ == "__main__":
	main()
