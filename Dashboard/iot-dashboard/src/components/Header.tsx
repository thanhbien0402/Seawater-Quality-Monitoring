'use client'
import { useTelemetry } from './context/TelemetryProvider'
import { Icons } from './icons'

export function Header() {
  const { name } = useTelemetry()

  return (
    <div className="flex flex-col justify-center md:items-start md:flex-row md:justify-between md:mt-3 md:mx-3">
      <div className="flex flex-col md:mt-0 text-center md:text-left ">
        <h1 className="text-xl md:text-4xl font-bold text-shadow-lg shadow-slate-800 uppercase">
          <span>Seawater Quality Monitor - Device: <span className='text-yellow-400'>{name}</span></span>

        </h1>
        <div className="flex justify-center md:justify-start items-center gap-1">
          <Icons.currentLocation className="w-6 h-6 md:block" />

          <p className="text-sm md:text-xl text-shadow shadow-slate-800 ">
            Phu Yen Province, Vietnam
          </p>
        </div>
      </div>
      <div className="flex flex-col  justify-center items-center text-center">
        <h1 className="text-xl md:text-3xl">Forecasts</h1>
        <Icons.cloudStorm className="w-6 h-6 md:w-12 md:h-12" />
      </div>
    </div>
  )
}
