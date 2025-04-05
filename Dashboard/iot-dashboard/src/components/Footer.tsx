'use client'
import { useEffect, useRef, useState } from 'react'
import { Card } from './Card'
import { Icons } from './icons'
import { Label } from '@/components/ui/label'
import { Switch } from '@/components/ui/switch'
import {
  Navigation,
  Pagination,
  Scrollbar,
  A11y,
  Autoplay,
  Parallax
} from 'swiper/modules'
import { Swiper, SwiperSlide } from 'swiper/react'

import 'swiper/css'
import 'swiper/css/navigation'
import 'swiper/css/pagination'
import { getTelemetryTb, loginTb, sendDataTb } from '@/actions'
import { readSetting } from '@/actions'
import { useToast } from '@/hooks/use-toast'
import { Calendar } from "@/components/ui/calendar"
import { TaskModal } from './modals/TaskModal'
import { createClient } from '@supabase/supabase-js'
import { before } from 'lodash'

const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL!;
const supabaseKey = process.env.NEXT_PUBLIC_SUPABASE_ANON_KEY!;
const supabase = createClient(supabaseUrl, supabaseKey);

export function Footer() {
  const [pH, setPH] = useState(0)
  const [ORP, setORP] = useState(0)
  const [TUR, setTUR] = useState(0)
  const [EC, setEC] = useState(0)

  const [switchState, setSwitchState] = useState<boolean>(false)
  const prevSwitchState = useRef<boolean>()

  const { toast } = useToast()
  const handleSwitch = (checked: boolean) => {
    prevSwitchState.current = switchState
    setSwitchState(checked)
  }

  supabase
    .channel('custom-channel')
    .on(
      'postgres_changes',
      { event: 'UPDATE', schema: 'public', table: 'tasks' },
      (payload) => {
        if (payload.new.status === 'success') {
          if (payload.new.trigger_type === 'true') {
            handleSwitch(true)
          } else {
            handleSwitch(false)
          }

          toast({
            title: "You submitted the following values:",
            description: (
              <pre className="mt-2 w-[340px] rounded-md bg-slate-950 p-4">
                <p className="text-white">Task Done Successfully: pumpStatus ---- ${payload.new.trigger_type === 'true' ? 'ON' : 'OFF'}</p>
              </pre>
            ),
          })
        }
      }
    )
    .subscribe();

  const [date, setDate] = useState<Date | undefined>(undefined)
  const [isOpen, isSetOpen] = useState(false)
  const handlePickDate = (newSelected: Date | undefined) => {
    isSetOpen(true)
    setDate(newSelected)
  }

  useEffect(() => {
    const fetchLoginData = async () => {

      try {
        const login = await loginTb()
        const setting: any = await readSetting()

        if (setting && setting.data && setting.data.length > 0) {
          const token = login.token as string
          localStorage.setItem('token', token)
          let { entityType, entityId } = setting.data[0]

          const webSocket = new WebSocket(
            process.env.NEXT_PUBLIC_TB_WS_URL || ''
          )

          webSocket.onopen = () => {
            const object = {
              authCmd: {
                cmdId: 0,
                token: token
              },
              cmds: [
                {
                  entityType: entityType,
                  entityId: entityId,
                  scope: 'LATEST_TELEMETRY',
                  cmdId: 10,
                  type: 'TIMESERIES'
                }
              ]
            }
            const data = JSON.stringify(object)
            webSocket.send(data)
          }

          webSocket.onmessage = (event) => {
            const receivedData = JSON.parse(event.data)
            const { subscriptionId, data } = receivedData
            if (data?.pH?.length > 0) {
              setPH(data.pH[0][1])
            }
            if (data?.ORP?.length > 0) {
              setORP(data.ORP[0][1])
            }
            if (data?.TUR?.length > 0) {
              setTUR(data.TUR[0][1])
            }
            if (data?.EC?.length > 0) {
              setEC(data.EC[0][1])
            }
          }

          webSocket.onclose = () => {
            console.log('Connection closed!')
          }

          return () => {
            webSocket.close()
          }
        } else {
          console.error('Setting data is null or empty')
        }
      } catch (error) {
        console.log(error)
      }
    }


    const getStatusSwitch = async () => {
      try {
        const setting: any = await readSetting()
        const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))

        if (setting && setting.data && setting.data.length > 0) {
          let { entityId } = setting.data[0]

          const data = await getTelemetryTb(entityId, token, ['pumpStatus'])

          if (data?.pumpStatus.length > 0) {
            setSwitchState(data.pumpStatus[0].value === '0' ? false : true)
          }
        }
      } catch (error) {
        console.log(error)
      }
    }
    getStatusSwitch()
    fetchLoginData()
  }, [])

  useEffect(() => {



    const sendData = async () => {
      const token = JSON.parse(JSON.stringify(localStorage.getItem('token')))
      await sendDataTb('control_pump', switchState, token)
    }

    console.log('current switch state', switchState)
    console.log('old switch state', prevSwitchState.current)

    if (prevSwitchState.current !== undefined && switchState !== undefined && prevSwitchState.current !== switchState) {
      console.log('call here')
      sendData()
    }
  }, [switchState])

  return (
    <div className="flex flex-col">
      <div className="flex justify-between mx-4">
        <div className="flex flex-col">
          <Calendar
            mode="single"
            selected={date}
            disabled={{ before: new Date() }}
            onSelect={handlePickDate}
            className="rounded-md border"
          />
          <TaskModal isOpen={isOpen} isSetOpen={isSetOpen} date={date} setDate={setDate} />
        </div>
        <div className="flex items-end">
          <div>
            <Switch id="airplane-mode" checked={switchState} onCheckedChange={handleSwitch} />
            <Label htmlFor="airplane-mode">
              Pump State: {switchState ? 'On' : 'Off'} Mode
            </Label>
          </div>
        </div>
      </div>
      <div className="flex m-4 gap-3 min-h-60">
        <Swiper
          modules={[
            Navigation,
            Pagination,
            Scrollbar,
            A11y,
            Autoplay,
            Parallax
          ]}
          spaceBetween={10}
          slidesPerView={4}
          navigation
          pagination={{ clickable: true }}
        // autoplay
        // breakpoints={{
        //   200: {
        //     slidesPerView: 1,
        //     spaceBetween: 10
        //   },
        //   640: {
        //     slidesPerView: 2,
        //     spaceBetween: 20
        //   },
        //   768: {
        //     slidesPerView: 3,
        //     spaceBetween: 30
        //   },
        //   1024: {
        //     slidesPerView: 4,
        //     spaceBetween: 10
        //   }
        // }}
        >
          <SwiperSlide>
            <Card
              title="pH"
              icon={<Icons.droplet />}
              value={pH}
              unit={''}
              texts={['', 'Seawater PH', '']}
            />
          </SwiperSlide>
          <SwiperSlide>
            <Card
              title="ORP"
              icon={<Icons.gauge />}
              value={ORP}
              unit={'mV'}
              texts={['', 'Seawater Oxidation-reduction Potential', '']}
            />
          </SwiperSlide>
          <SwiperSlide>
            <Card
              title="TUR"
              icon={<Icons.cloudRain />}
              value={TUR}
              unit={'NTU'}
              texts={['', 'Seawater Turbidity', '']}
            />
          </SwiperSlide>
          <SwiperSlide>
            <Card
              title="EC"
              icon={<Icons.temperature />}
              value={EC}
              unit={'μS/cm'}
              texts={['', 'Seawater Electrical Conductivity', '']}
            />
          </SwiperSlide>
        </Swiper>
      </div>
    </div>
  )
}
