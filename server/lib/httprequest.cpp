#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QObject>
#include <QStringList>
#include <QUrlQuery>
#include "httprequest.h"
#include "log.h"

HTTPRequest::HTTPRequest(QByteArray const& data):type(INVALID)
{
	if (data.size() < 4 || *(int*)data.left(4).constData() != data.size())
	{
		LogError("HTTP Request : Invalid data");
		return;
	}
	RequestType t = (RequestType)data.at(4);
	QByteArray content = data.mid(5);
	switch (t)
	{
		case GET:
			rawHeaders = content.left(content.indexOf('\0')); // Copy headers, stop at first \x00
			rawUri = content.mid(rawHeaders.length()+1); // Copy URI
			type = GET;
			break;
			
		case POST:
		{
			rawHeaders = content.left(content.indexOf('\0')); // Copy headers, stop at first \x00
			content = content.mid(rawHeaders.length()+1);
			rawUri = content.left(content.indexOf('\0')); // Copy URI
			rawPostData = content.mid(rawUri.length()+1);
			// Parse Post Data
			QList<QByteArray> listOfParams = rawPostData.split('&');
			foreach(QByteArray param, listOfParams)
			{
				QByteArray key = param.left(param.indexOf('='));
				QByteArray value = param.mid(param.indexOf('=')+1);
				formPostData[QUrl::fromPercentEncoding(key)] = QUrl::fromPercentEncoding(value);
			}
			type = POST;
			break;
		}
		
		case POSTRAW:
			rawHeaders = content.left(content.indexOf('\0')); // Copy headers, stop at first \x00
			content = content.mid(rawHeaders.length()+1);
			rawUri = content.left(content.indexOf('\0')); // Copy URI
			rawPostData = content.mid(rawUri.length()+1);
			type = POSTRAW;
			break;
	
		default:
			LogError("HTTP Request : Invalid type");
			return;
	}

	// Parse URI
	QUrl uriInst(rawUri);

    QUrlQuery url(uriInst.query());
    uri = uriInst.path();

    if(!url.isEmpty() && uriInst.isValid())
	{
        QList<QPair<QString, QString> > items = url.queryItems();
        for (QList< QPair<QString, QString> >::iterator i = items.begin(); i != items.end(); i++) {
        	getData[i->first] = i->second;
        }
	} else
		LogError("rawUri is empty or invalid");
}

QString HTTPRequest::toString() const
{
	// QString s;
	// s.append(QString("<ul><li>URL : %1</li>").arg(QString(uri)));
	// s.append("<li>Get Args : <br /><ul>");
	// foreach (QString str, getData.keys())
	// 	s.append(QString("<li>%1 => %2</li>").arg(str,getData.value(str)));
	// s.append("</ul></li>");
	// if(type == POST)
	// {
	// 	s.append("<li>Post Args : <br /><ul>");
	// 	foreach (QString str, formPostData.keys())
	// 		s.append(QString("<li>%1 => %2</li>").arg(str,formPostData.value(str)));
	// 	s.append("</ul></li>");
	// }
	// s.append("</ul>");
	// return s;

	QString s;
	// URL: '' ARGS: ['',''...], POST
	s.append(QString("URL: '%1', GET ARGS: ").arg(QString(uri)));
	foreach (QString str, getData.keys())
		s.append(QString("['%1', '%2'] ").arg(str, getData.value(str)));
	if (type == POST) {
		s.append(QString(" POST ARGS: "));
		foreach (QString str, formPostData.keys())
			s.append(QString("['%1', '%2'] ").arg(str, formPostData.value(str)));
	}
	return s;

}
