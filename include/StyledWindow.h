#ifndef STYLEDWINDOW_H
#define STYLEDWINDOW_H

#include <QCloseEvent>
#include <QPaintEvent>
#include <QWidget>
#include <atomic>

class StyledWindow : public QWidget
{
  Q_OBJECT
public:
  StyledWindow(QWidget *parent = nullptr);

  void
  allowClose(const bool &allow);

protected:
  virtual void
  paintEvent(QPaintEvent *event) override;

  virtual void
  closeEvent(QCloseEvent *event) override;

  std::atomic<bool> allow_close = true;
};

#endif // STYLEDWINDOW_H
